class MEAPSynthProcessor extends AudioWorkletProcessor {
    static get parameterDescriptors() {
        return [{
            name: 'frequency',
            defaultValue: 440,
            minValue: 20,
            maxValue: 20000
        }, {
            name: 'cutoff',
            defaultValue: 1000,
            minValue: 20,
            maxValue: 20000
        }, {
            name: 'resonance',
            defaultValue: 1,
            minValue: 0.1,
            maxValue: 10
        }];
    }

    constructor() {
        super();
        this.phase = 0;
    }

    process(inputs, outputs, parameters) {
        const output = outputs[0];
        const frequency = parameters.frequency[0];
        const cutoff = parameters.cutoff[0];
        const resonance = parameters.resonance[0];

        for (let i = 0; i < output[0].length; i++) {
            const sample = Math.sin(2 * Math.PI * this.phase);
            this.phase += frequency / sampleRate;
            if (this.phase > 1) this.phase -= 1;
            
            output.forEach(channel => {
                channel[i] = sample;
            });
        }
        return true;
    }
}

registerProcessor('meap-synth-processor', MEAPSynthProcessor);
