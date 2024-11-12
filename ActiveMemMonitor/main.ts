import express from "express";
import { WebSocket, WebSocketServer } from "ws";
import { v4 as uuidv4 } from "uuid";
import net from "net";

const PIPE_PATH = "\\\\.\\pipe\\active_mem_monitor";

async function initializePipeServer(ws: WebSocketServer) {
    const server = net.createServer(function(stream) {
        console.log('Server: on connection')

        stream.on('data', function(c) {
            let message = c.toString();
            console.log('Server: on data:', message);
            if(message.includes("HeapAlloc")) {
                let chunks  = message.split(",");
                let hHeap   = chunks[0].split("(")[1].trim();
                let ret     = chunks[chunks.length - 1].split(" ").pop()?.trim().replace("\u0000","");
                //HeapAlloc(0x5a840000,0x0,0x30) -> 0x17762b97c90600
                for(let wsc of ws.clients) {
                    wsc.send(`{"heap":"${hHeap}","ret":"${ret}"}`);
                }
            }
            
        });

        stream.on('end', function() {
            console.log('Server: on end');
        });
    });

    server.on('close',function(){
        console.log('Server: on close');
    });

    server.listen(PIPE_PATH,function(){
        console.log('Server: on listening');
    });
};

async function initializeWebServer() {
    const app = express();

    app.use(express.static("static"));
    
    app.listen(8000, () => {
        console.log("Listening on port 8000");
    });

}

async function initializeWebSocketServer() {
    const ws = new WebSocketServer({ port: 9000 });

    ws.on("error", console.error);
    
    ws.on("connection", (wsc: {[id: string]: string} & WebSocket) => {
        wsc["id"] = uuidv4();
        console.log(wsc["id"]);
        
        wsc.on("error", console.error);
        wsc.on("message", (e: any) => {
            console.log(`[${wsc["id"]}]: ${e}`);
            wsc.send(String(e));
        });
    
        // wsc.send("ECHO SERVER HELLO");
    });
    
    return ws;
}

async function main() {
    const ws = await initializeWebSocketServer();
    await initializePipeServer(ws);
    await initializeWebServer();
    
    
    console.log("xd");

    // for await (const [msg] of pull) {
    //     console.log(`IPC: ${msg.toString()}`)
    //     ws.clients.forEach((client) => {
    //         client.send(JSON.stringify(msg));
    //     });
    // }
}

main();