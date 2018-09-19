const request = require("request");
const rpromise = require("request-promise");
const eosjs = require("eosjs");

const APIs = [
    "https://api.binance.com/api/v1/ticker/price?symbol=EOSUSDT",
    "https://api.bitfinex.com/v1/pubticker/eosusd",
    "https://api.huobi.pro/market/detail/merged?symbol=eosusdt",
    "https://www.okex.com/api/v1/ticker.do?symbol=eos_usdt"
];

const config = {
    //set private-key
    keyProvider: ["private-key"],
    //set account name
    authorization: "account-name@active",
    httpEndpoint: "http://127.0.0.1:8888",
    expireInSeconds: 60,
    broadcast: true,
    debug: false,
    verbose: false,
    sign: true
};

const eos = eosjs(config);

let contract;

function main() {
    //TODO: Verify blockchain consistency 
    //eos.getInfo({}).then(r => console.log(r));

    eos.contract("price.update").then((data) => {
        contract = data;
        updatePrice();
        setInterval(() => {
            updatePrice();
        }, 30 * 1000);
    });
}

function updatePrice() {
    get_prices().then((arr) => {
        contract.update({
                ticker_name: "eos.usd",
                prices: arr,
                avg_price: avg(arr),
                timestamp: new Date().toGMTString()
            })
            .then(result => {
                console.log(result);
                setTimeout(() => {
                    read_price();
                }, 1000);

            })
            .catch(e => console.log(e));
    });
}

function get_prices() {
    return new Promise((resolve, reject) => {
        const queue = [];
        APIs.forEach(a => queue.push(request_data(a)));

        Promise.all(queue).then((result) => {
            const binance = result[0]["price"];
            const bitfinex = result[1]["last_price"];
            const huobi = result[2]["tick"]["close"];
            const okex = result[3]["ticker"]["last"];

            resolve([binance, bitfinex, huobi, okex].map(x => parseFloat(x)));
        }, (err) => {
            console.log(err);
        });
    });
}

function request_data(url) {
    return rpromise({
        uri: url,
        json: true
    });
}

function avg(arr) {
    let sum = 0;
    arr.forEach((v) => {
        sum += v
    });
    return sum / arr.length;
}

function read_price() {
    eos.getTableRows(true, "price.update", "price.update", "ticker").then((data) => {
        console.log(data);
    });
}

main();
