const { Encoder, Decoder } = require('./build/Release/heatshrink');

let encoder = new Encoder({ windowSize: 8, lookaheadSize: 4 });
let decoder = new Decoder({ windowSize: 8, lookaheadSize: 4 });

let raw = '';

encoder.on('data', buf => {
    decoder.write(buf);
});

encoder.on('end', () => {
    decoder.end();
});

let body = '';

decoder.on('data', buf => {
    body += buf.toString();
});

decoder.on('end', () => {
    console.log('Decoding complete.');
    if (body !== raw) {
        console.log(' - result does not match');
    } else {
        console.log(' - result matches');
    }
});

let msg = '1234567890';
for (let i = 0; i < 500; i++) {
    raw += msg;
    encoder.write(msg);
}
encoder.end();
