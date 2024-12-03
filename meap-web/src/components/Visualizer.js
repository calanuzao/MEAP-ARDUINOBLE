class Visualizer extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({ mode: 'open' });
        
        const container = document.createElement('div');
        container.style.cssText = `
            width: 100%;
            height: 100%;
            background: #ffffff;
            border-radius: 4px;
            border: 2px solid #000000;
        `;
        
        this.canvas = document.createElement('canvas');
        this.canvas.style.cssText = `
            width: 100%;
            height: 100%;
        `;
        
        this.ctx = this.canvas.getContext('2d');
        container.appendChild(this.canvas);
        this.shadowRoot.appendChild(container);
        
        window.addEventListener('resize', () => {
            this.resizeCanvas();
        });
        
        this.resizeCanvas();
    }

    resizeCanvas() {
        this.canvas.width = this.clientWidth || 300;
        this.canvas.height = this.clientHeight || 150;
    }

    update(dataArray) {
        if (!dataArray || dataArray.length === 0) return;
        
        const width = this.canvas.width;
        const height = this.canvas.height;
        
        // Clear the canvas
        this.ctx.fillStyle = '#ffffff';
        this.ctx.fillRect(0, 0, width, height);
        
        // Draw center line
        this.ctx.strokeStyle = '#cccccc';
        this.ctx.lineWidth = 1;
        this.ctx.beginPath();
        this.ctx.moveTo(0, height / 2);
        this.ctx.lineTo(width, height / 2);
        this.ctx.stroke();
        
        // Draw waveform
        this.ctx.strokeStyle = '#000000';
        this.ctx.lineWidth = 1.5;  // Slightly thinner line for elegance
        this.ctx.beginPath();
        
        // Smooth the data first
        const smoothedData = [];
        const smoothingFactor = 3;  // Adjust for more/less smoothing
        
        for (let i = 0; i < dataArray.length; i++) {
            let sum = 0;
            let count = 0;
            for (let j = Math.max(0, i - smoothingFactor); j <= Math.min(dataArray.length - 1, i + smoothingFactor); j++) {
                sum += dataArray[j];
                count++;
            }
            smoothedData[i] = sum / count;
        }
        
        // Calculate the average of smoothed data
        const avg = smoothedData.reduce((a, b) => a + b, 0) / smoothedData.length;
        
        // Scale factor to zoom out (much smaller for more zoom out)
        const scaleFactor = 0.15;  // Reduced from 0.5 to 0.15
        
        // Draw the smoothed waveform
        const sliceWidth = width / smoothedData.length;
        let x = 0;
        
        for (let i = 0; i < smoothedData.length; i++) {
            const normalizedValue = (smoothedData[i] - avg) / 128.0;
            const y = height / 2 + (normalizedValue * height * scaleFactor);
            
            if (i === 0) {
                this.ctx.moveTo(x, y);
            } else {
                // Use quadratic curves for smoother lines
                const prevX = x - sliceWidth;
                const prevY = height / 2 + ((smoothedData[i-1] - avg) / 128.0 * height * scaleFactor);
                const cpX = (x + prevX) / 2;
                this.ctx.quadraticCurveTo(cpX, prevY, x, y);
            }
            
            x += sliceWidth;
        }
        
        this.ctx.stroke();
    }
}

customElements.define('audio-visualizer', Visualizer);