
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
