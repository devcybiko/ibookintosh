function main() {
    process.stdin.setRawMode(true);
    process.stdin.resume();
    process.stdin.setEncoding('utf8');

    let keys = [];
    for (let i=0; i<40; i++) keys[i] = 0;
    let buff = "";

    process.stdin.on('data', (key) => {
        // Ctrl+C to exit
        if (key === '\u0003') {
            process.exit();
        }
        if ([32, 9, 10, 13, 27].includes(key.charCodeAt(0))) {
            let n = Number(buff);
            if (1 <= n && n <=40) {
                keys[n-1]++;
                console.log("PIN:", n);
            } else {
                console.error("ERROR: " + n);
            }
            buff = "";
            let s = "";
            for(let i=0; i<40; i++) s += (String(i+1).padStart(4, ' '));
            console.log(s);
            s = ""
            for(let i=0; i<40; i++) s += (keys[i] ? String(keys[i]).padStart(4, ' ') : "    ");
            console.log(s);
            console.log();
        } else {
            if ('0' <= key && key <= '9') buff += key;
            else (console.error("KEY:", key.charCodeAt(0)))
        }
    });

}

main();