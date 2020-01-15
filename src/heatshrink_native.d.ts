
declare interface encoderOpts {
	windowSize: number;
	lookaheadSize: number;
};

declare interface decoderOpts {
	windowSize: number;
	lookaheadSize: number;
};

export class HSEncoder {
	constructor(opts: encoderOpts);
	sink(arg0: Buffer): number;
	poll();
	end();
}

export class HSDecoder {
	constructor(opts: decoderOpts);
	sink(arg0: Buffer): number;
	poll();
	end();
}

export = { HSEncoder, HSDecoder };
