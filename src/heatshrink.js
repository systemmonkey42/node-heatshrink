const heatshrink = require('./heatshrink_native');
const { EventEmitter } = require('events');

class Encoder extends EventEmitter {
    /** @param {heatshrink.encoderOpts} opts */
    constructor(opts) {
        this.opts = opts || {};
    }
}

class Decoder extends EventEmitter {
    /** @param {decoderOpts} opts */
    constructor(opts) {
        this.opts = opts || {};
    }
}

module.exports = { Encoder, Decoder };
