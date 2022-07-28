
async function updateUI() {
    return new Promise(resolve => setTimeout(resolve, 30));
}

async function load_wasm(wasm_binary: ArrayBuffer) {

    return new Promise((resolve) => {
        const worker = new Worker('dist/worker.js');
        worker.onmessage = ({ data }) => {
            const [id, message, value] = data;
            const element = document.getElementById(id);
            if (element) {
                const textArea = element as HTMLTextAreaElement;
                switch (message) {
                    case 'set':
                        textArea.value = value;
                        break;
                    case 'append':
                        textArea.value += value;
                        break;
                    case 'final':
                        textArea.value += value;
                        worker.terminate();
                        resolve('Done');
                }
            }
        }
        worker.postMessage(wasm_binary);
    });
}