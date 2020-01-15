
declare interface encoderOpts {
	windowSize: number;
	lookaheadSize: number;
};

declare interface decoderOpts {
	windowSize: number;
	lookaheadSize: number;
};

export class Encoder {
	constructor(opts: encoderOpts);
	on(event: 'data', cb: (arg0: Buffer) => void):this;
	test();
	read(): Buffer;
	write(data: Buffer);
	flush();
	close();
}

export class Decoder {
	constructor(opts: decoderOpts);
	read(): Buffer;
	write(data: Buffer);
	flush();
	close();
}

export = { Encoder, Decoder };
