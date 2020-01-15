
import * as stream from 'stream';

declare interface encoderOpts {
	windowSize: number;
	lookaheadSize: number;
}

declare interface decoderOpts {
	windowSize: number;
	lookaheadSize: number;
}

export class Encoder extends stream.Transform {
	constructor(opts: encoderOpts);
	on(event: 'data', cb: (arg:Buffer)=>void):this;
	on(event: 'readable', cb: ()=>void):this;
	on(event: 'end', cb: ()=>void):this;
	test();
	read(): Buffer;
	write(data: string|Buffer);
	flush();
	close();
}

export class Decoder extends stream.Transform {
	constructor(opts: decoderOpts);
	on(event: 'data', cb: (arg:Buffer)=>void):this;
	on(event: 'readable', cb: ()=>void):this;
	on(event: 'end', cb: ()=>void):this;
	read(): Buffer;
	write(data: string|Buffer);
	flush();
	close();
}
