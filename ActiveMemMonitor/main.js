"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const ws_1 = require("ws");
const uuid_1 = require("uuid");
const net_1 = __importDefault(require("net"));
const PIPE_PATH = "\\\\.\\pipe\\active_mem_monitor";
function initializePipeServer(ws) {
    return __awaiter(this, void 0, void 0, function* () {
        const server = net_1.default.createServer(function (stream) {
            console.log('Server: on connection');
            stream.on('data', function (c) {
                console.log('Server: on data:', c.toString());
            });
            stream.on('end', function () {
                console.log('Server: on end');
                server.close();
            });
        });
        server.on('close', function () {
            console.log('Server: on close');
        });
        server.listen(PIPE_PATH, function () {
            console.log('Server: on listening');
        });
    });
}
;
function initializeWebServer() {
    return __awaiter(this, void 0, void 0, function* () {
        const app = (0, express_1.default)();
        app.use(express_1.default.static("static"));
        app.listen(8000, () => {
            console.log("Listening on port 8000");
        });
    });
}
function initializeWebSocketServer() {
    return __awaiter(this, void 0, void 0, function* () {
        const ws = new ws_1.WebSocketServer({ port: 9000 });
        ws.on("error", console.error);
        ws.on("connection", (wsc) => {
            wsc["id"] = (0, uuid_1.v4)();
            console.log(wsc["id"]);
            wsc.on("error", console.error);
            wsc.on("message", (e) => {
                console.log(`[${wsc["id"]}]: ${e}`);
                wsc.send(String(e));
            });
            wsc.send("ECHO SERVER HELLO");
        });
        return ws;
    });
}
function main() {
    return __awaiter(this, void 0, void 0, function* () {
        const ws = yield initializeWebSocketServer();
        yield initializePipeServer(ws);
        yield initializeWebServer();
        console.log("xd");
        // for await (const [msg] of pull) {
        //     console.log(`IPC: ${msg.toString()}`)
        //     ws.clients.forEach((client) => {
        //         client.send(JSON.stringify(msg));
        //     });
        // }
    });
}
main();
