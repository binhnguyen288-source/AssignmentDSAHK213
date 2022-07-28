



type i8 = number;
type u8 = number;
type i32 = number;
type u32 = number;
type i64 = bigint;

onmessage = async (ev) => {
    const wasm_binary = ev.data as ArrayBuffer;
    const fileIDs = [
        "", 
        "consoleout",
        "", 
        "testcase",
        "expected",
        "got"
    ];



    fileIDs.forEach(v => {
        postMessage([v, "set", ""]);
    });
    const fileContents = ['', '', '', '', '', ''];
    const fileOffsets = [0, 0, 0, 0, 0, 0];
    let memory : WebAssembly.Memory;
    const textDecoder = new TextDecoder();
    const imports = {
        env: {
            emscripten_notify_memory_growth: () => 0,
            signal_open: (fd: number, reset: number) => {
                fileOffsets[fd] = 0;
                if (reset) {
                    fileContents[fd] = '';
                    postMessage([fileIDs[fd], "set", ""]);
                }
                return 0;
            }
        },
        wasi_snapshot_preview1: {
            proc_exit: (code: i32) => {
                throw code;
            },
            fd_seek: (fd: i32, offset: i64, whence: i8, newoffsetPtr: u32) => {
                switch (whence) {
                    case 0: // from start
                        fileOffsets[fd] = Number(offset);
                        break;
                    case 1: // from current
                        fileOffsets[fd] += Number(offset);
                        break;
                    case 2: // from end
                        fileOffsets[fd] = fileContents[fd].length + Number(offset);
                        break;
                }
                (new DataView(memory.buffer)).setBigInt64(newoffsetPtr, BigInt(fileOffsets[fd]), true);
                return 0;
            },
            fd_write: (fd: i32, iovsPtr: u32, iov: u32, nwrittenPtr: u32): number => {
                const memU32 = new Uint32Array(memory.buffer);

                let nwritten = 0;
                let str = '';

                for (let i = 0; i < iov; ++i) {
                    const buf = memU32[(iovsPtr >> 2) + 2 * i];
                    const size = memU32[(iovsPtr >> 2) + 2 * i + 1];
                    nwritten += size;
                    const jsbuf = new Uint8Array(memory.buffer, buf, size);
                    str += textDecoder.decode(jsbuf);
                }

                fileContents[fd] += str;
                postMessage([fileIDs[fd], "append", str]);

                memU32[nwrittenPtr>>2] = nwritten;

                return 0;
            },
            fd_read: (fd: i32, iovsPtr: u32, iov: u32, nreadPtr: u32) => {
                if (fd !== 3) throw "Error, reading an invalid file";
                const memoryView = new DataView(memory.buffer);
                let nread = 0;
                for (let i = 0; i < iov; ++i) {
                    const buf = memoryView.getUint32(iovsPtr + 8 * i, true);
                    const size = memoryView.getUint32(iovsPtr + 8 * i + 4, true);
                    for (let j = 0; j < size && fileOffsets[fd] < fileContents[fd].length; ++j, ++fileOffsets[fd], ++nread) {
                        memoryView.setUint8(buf + j, fileContents[fd].charCodeAt(fileOffsets[fd]));
                    }
                }
                memoryView.setUint32(nreadPtr, nread, true);
                return 0;
            },
            fd_close: (fd: i32) => 0,
            environ_sizes_get: (argcPtr: u32, argvBufSizePtr: u32) => {
                const memU32 = new Uint32Array(memory.buffer);
                memU32[argcPtr >> 2] = 0;
                memU32[argvBufSizePtr >> 2] = 0;
                return 0;
            },
            args_sizes_get: (argcPtr: u32, argvBufSizePtr: u32) => {
                const memU32 = new Uint32Array(memory.buffer);
                memU32[argcPtr >> 2] = 0;
                memU32[argvBufSizePtr >> 2] = 0;
                return 0;
            },
            environ_get: (argv: u32, argvBuf: u32) => 0,
            args_get: (argv: u32, argvBuf: u32) => 0,
            random_get: (bufPtr: number, length: number) => {
                crypto.getRandomValues(new Uint8Array(memory.buffer, bufPtr, length));
                return 0;
            },
            clock_time_get: (id: number, precision: bigint, timePtr: number) => {
                const memoryView = new DataView(memory.buffer);
                memoryView.setBigUint64(timePtr, BigInt(Math.round(performance.now() * 1000000 / Number(precision))), true);
                return 0;
            }
        }
    };
   

    try {
        const { instance } = await WebAssembly.instantiate(wasm_binary, imports);
        memory = instance.exports.memory as WebAssembly.Memory;
        (instance.exports._start as Function)();
    }
    catch (e) {
        postMessage([fileIDs[1], "final", `\nExited with code: ${e}`]);
    }
}