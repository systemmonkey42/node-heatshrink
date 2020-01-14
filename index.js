const { Encoder, Decoder } = require('./build/Release/heatshrink');

console.log('Encoder =', Encoder);
console.log('Decoder =', Decoder);
let encoder = new Encoder({ windowSize: 8, lookaheadSize: 4 });

encoder.close();
console.log('encoder instance =', encoder);
//encoder.close();

//let decoder = new Decoder({ windowSize: 8, lookaheadSize: 4 });

//decoder.close();
