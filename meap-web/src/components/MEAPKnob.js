class MEAPKnob extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({ mode: 'open' });
        
        this.value = 0;
        this.knobId = this.getAttribute('knob-id');
        this.label = this.getAttribute('label') || `Pot ${this.knobId}`;
    }

    connectedCallback() {
        this.shadowRoot.innerHTML = `
            <style>
                .knob-container {
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    margin: 10px;
                }
                .knob {
                    width: 100px;
                    height: 100px;
                    border-radius: 50%;
                    background: #444;
                    position: relative;
                    cursor: pointer;
                    transition: transform 0.1s ease;
                    transform-origin: center;
                }
                .knob::after {
                    content: '';
                    position: absolute;
                    width: 4px;
                    height: 50%;
                    background: #fff;
                    left: 50%;
                    transform-origin: bottom;
                    transform: translateX(-50%) rotate(${this.value}deg);
                }
                .label {
                    margin-top: 10px;
                    font-family: sans-serif;
                }
                .value {
                    margin-top: 5px;
                    font-family: sans-serif;
                    font-weight: bold;
                }
            </style>
            <div class="knob-container">
                <div class="knob" id="knob-${this.knobId}"></div>
                <div class="label">${this.label}</div>
                <div class="value" id="value-${this.knobId}">Value: 0</div>
            </div>
        `;

        this.setupKnobEvents();
    }

    setupKnobEvents() {
        const knob = this.shadowRoot.querySelector('.knob');
        let isDragging = false;
        let startY;
        let currentValue = 0;
        const sensitivityFactor = 5;

        knob.addEventListener('mousedown', (e) => {
            isDragging = true;
            startY = e.clientY;
            e.preventDefault();
        });

        document.addEventListener('mousemove', (e) => {
            if (!isDragging) return;
            
            const deltaY = startY - e.clientY;
            currentValue = Math.max(0, Math.min(4095, currentValue + deltaY * sensitivityFactor));
            startY = e.clientY;
            
            this.setValue(currentValue);
            this.dispatchEvent(new CustomEvent('knob-change', {
                detail: { value: currentValue, id: this.knobId }
            }));
        });

        document.addEventListener('mouseup', () => {
            isDragging = false;
        });
    }

    setValue(value) {
        this.value = value;
        const maxPotValue = 4095; // Maximum value of the potentiometer
        const rotation = (value / maxPotValue) * 270; // 270 degrees of rotation
        const knob = this.shadowRoot.querySelector('.knob');
        const valueDisplay = this.shadowRoot.querySelector(`#value-${this.knobId}`);
        
        if (knob) {
            knob.style.transform = `rotate(${rotation}deg)`; // Rotate the knob
        }
        
        if (valueDisplay) {
            valueDisplay.textContent = `Value: ${value}`; // Update the displayed value
        }
    }
}

customElements.define('meap-knob', MEAPKnob);
