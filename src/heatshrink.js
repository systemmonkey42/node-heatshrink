const { HSEncoder, HSDecoder } = require('./heatshrink_native');
const { Transform } = require('stream');

/**
 * @typedef {object} encoderOpts
 * @property {number} [windowSize]
 * @property {number} [lookaheadSize]
 */

/**
 * @typedef {object} decoderOpts
 * @property {number} [windowSize]
 * @property {number} [lookaheadSize]
 */

/**
 * @typedef {(err?: Error|undefined|null)=>void} errCallback
 */
/**
 * @typedef {(err?: Error|undefined|null, arg?: T)=>void} Callback<T>
 * @template T
 */
/**
 * @typedef {Buffer} chunkType
 */

class Encoder extends Transform {
    /** @param {encoderOpts} opts */
    constructor(opts) {
        super();

        this.hse = new HSEncoder({
            windowSize: opts.windowSize || 8,
            lookaheadSize: opts.lookaheadSize || 4
        });
    }

    /**
     * @param {chunkType} chunk
     * @param {string|undefined} _encoding
     * @param {Callback<chunkType>} cb
     */
    _transform(chunk, _encoding, cb) {
        let len = chunk.length;
        let offset = 0;

        while (offset < len) {
            let bytes = this.hse.sink(chunk.slice(offset));

            if (bytes > 0) {
                offset += bytes;
                while (1) {
                    let r = this.hse.poll();
                    if (r && r.length) {
                        this.push(r);
                    } else {
                        break;
                    }
                }
            } else {
                throw new Error('failed during compression');
            }
        }
        cb(null, undefined);
    }

    /**
     * @param {errCallback} [cb]
     */
    _flush(cb) {
        let res = this.hse.end();

        if (res === 1) {
            while (1) {
                let r = this.hse.poll();
                if (r.length) {
                    this.push(r);
                } else {
                    break;
                }
            }
        }
        cb && cb(null);
    }
}

class Decoder extends Transform {
    /** @param {decoderOpts} opts */
    constructor(opts) {
        super();

        this.hsd = new HSDecoder({
            windowSize: opts.windowSize || 8,
            lookaheadSize: opts.lookaheadSize || 4
        });
    }

    /**
     * @param {chunkType} chunk
     * @param {string|undefined} _encoding
     * @param {Callback<chunkType>} cb
     */
    _transform(chunk, _encoding, cb) {
        let len = chunk.length;
        let offset = 0;

        while (offset < len) {
            let bytes = this.hsd.sink(chunk.slice(offset));

            if (bytes > 0) {
                offset += bytes;
                while (1) {
                    let r = this.hsd.poll();
                    if (r && r.length) {
                        this.push(r);
                    } else {
                        break;
                    }
                }
            } else {
                throw new Error('failed during decompression');
            }
        }
        cb(null, undefined);
    }

    /**
     * @param {errCallback} [cb]
     */
    _flush(cb) {
        let res = this.hsd.end();

        if (res === 1) {
            while (1) {
                let r = this.hsd.poll();
                if (r.length) {
                    this.push(r);
                } else {
                    break;
                }
            }
        }
        cb && cb();
    }
}

module.exports = { Encoder, Decoder };
