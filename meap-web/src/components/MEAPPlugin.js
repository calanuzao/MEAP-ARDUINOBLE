class MEAPPlugin extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({ mode: 'open' });
        this.bleDevice = null;
        this.bleService = null;
        this.characteristics = {};
    }

    connectedCallback() {
        this.shadowRoot.innerHTML = `
            <style>
                .container {
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    padding: 20px;
                }
                .knobs {
                    display: flex;
                    justify-content: space-around;
                    width: 100%;
                }
                button {
                    margin: 20px;
                    padding: 10px 20px;
                }
            </style>
            <div class="container">
                <button id="connect">Connect MEAP</button>
                <div class="knobs">
                    <meap-knob knob-id="1" label="Pot 1"></meap-knob>
                    <meap-knob knob-id="2" label="Pot 2"></meap-knob>
                    <meap-knob knob-id="3" label="Pot 3"></meap-knob>
                </div>
            </div>
        `;

        this.setupEvents();
    }

    async setupEvents() {
        const connectButton = this.shadowRoot.querySelector('#connect');
        connectButton.addEventListener('click', () => this.connectBLE());

        this.shadowRoot.querySelectorAll('meap-knob').forEach(knob => {
            knob.addEventListener('knob-change', (e) => this.handleKnobChange(e));
        });
    }

    async connectBLE() {
        try {
            this.bleDevice = await navigator.bluetooth.requestDevice({
                filters: [{ name: 'MEAP' }],
                optionalServices: ['a597fb2d-5947-5bb6-9b20-9fa9ec68916f']
            });

            const server = await this.bleDevice.gatt.connect();
            this.bleService = await server.getPrimaryService('a597fb2d-5947-5bb6-9b20-9fa9ec68916f');

            // Get characteristics for each pot
            const characteristicUUIDs = [
                'a597fb2e-5947-5bb6-9b20-9fa9ec68916f', // pot1
                'a597fb2f-5947-5bb6-9b20-9fa9ec68916f', // pot2
                'a597fb30-5947-5bb6-9b20-9fa9ec68916f'  // pot3
            ];

            // Get characteristics for each pot
            this.characteristics = {
                pot1: await this.bleService.getCharacteristic(characteristicUUIDs[0]),
                pot2: await this.bleService.getCharacteristic(characteristicUUIDs[1]),
                pot3: await this.bleService.getCharacteristic(characteristicUUIDs[2])
            };

            console.log('Connected to MEAP!');
        } catch (error) {
            console.error('BLE Connection failed:', error);
        }
    }

    async handleKnobChange(event) {
        if (!this.bleService) return;

        const { value, id } = event.detail;
        const characteristic = this.characteristics[`pot${id}`];
        
        if (characteristic) {
            try {
                await characteristic.writeValue(new TextEncoder().encode(value.toString()));
            } catch (error) {
                console.error(`Failed to send pot${id} value:`, error);
            }
        }
    }
}

customElements.define('meap-plugin', MEAPPlugin);