class FileExplorer extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({ mode: 'open' });

        this.visualizer = document.querySelector('audio-visualizer');
        this.fileInput = document.createElement('input');
        this.fileInput.type = 'file';
        this.fileInput.accept = 'audio/*';
        this.playButton = document.createElement('button');
        this.playButton.textContent = 'Play';
        this.audioContext = null;
        this.analyser = null;
        this.source = null;
        this.audioBuffer = null;
        this.isPlaying = false;
        this.animationId = null;

        const container = document.createElement('div');
        container.style.display = 'flex';
        container.style.flexDirection = 'column';
        container.style.alignItems = 'center';
        container.style.gap = '20px';

        const controlsContainer = document.createElement('div');
        controlsContainer.style.display = 'flex';
        controlsContainer.style.alignItems = 'center';
        controlsContainer.style.gap = '10px';

        controlsContainer.appendChild(this.fileInput);
        controlsContainer.appendChild(this.playButton);

        container.appendChild(controlsContainer);
        this.shadowRoot.appendChild(container);

        this.setupEvents();
    }

    setupEvents() {
        this.fileInput.addEventListener('change', (event) => this.handleFileUpload(event));
        this.playButton.addEventListener('click', () => this.togglePlayPause());
    }

    async initAudio() {
        this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
        this.analyser = this.audioContext.createAnalyser();
        this.analyser.fftSize = 2048;

        const source = this.audioContext.createBufferSource();
        source.buffer = this.audioBuffer;
        source.connect(this.analyser);
        this.analyser.connect(this.audioContext.destination);

        source.start(0);
        this.visualizeAudio();
    }

    async handleFileUpload(event) {
        this.initAudio();

        const file = event.target.files[0];

        if (!file) {
            console.error("No file selected");
            return;
        }

        const reader = new FileReader();

        reader.onload = (e) => {
            const arrayBuffer = e.target.result;
            this.audioContext.decodeAudioData(arrayBuffer, (buffer) => {
                this.audioBuffer = buffer;
                this.playButton.disabled = false;
            }, (error) => {
                console.error("Error decoding audio data:", error);
            });
        };

        reader.readAsArrayBuffer(file);
    }

    togglePlayPause() {
        if (!this.audioContext) {
            this.initAudio();
        }

        if (this.audioContext.state === 'suspended') {
            this.audioContext.resume();
        }

        if (!this.audioBuffer) {
            console.error("No audio file loaded");
            return;
        }

        if (this.isPlaying) {
            this.source.stop();
            cancelAnimationFrame(this.animationId);
            this.isPlaying = false;
            this.playButton.textContent = 'Play';
        } else {
            this.source = this.audioContext.createBufferSource();
            this.source.buffer = this.audioBuffer;
            this.source.connect(this.analyser);
            this.analyser.connect(this.audioContext.destination);
            this.source.start(0);
            this.visualizeAudio();
            this.isPlaying = true;
            this.playButton.textContent = 'Pause';
        }
    }

    visualizeAudio() {
        const dataArray = new Uint8Array(this.analyser.frequencyBinCount);
        const draw = () => {
            this.analyser.getByteTimeDomainData(dataArray);
            this.visualizer.update(dataArray);
            this.animationId = requestAnimationFrame(draw);
        };
        draw();
    }
}

customElements.define('file-explorer', FileExplorer);