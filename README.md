# EOS Rio Price Oracle

This is a simple service that can store information of different exchange rates in a Multi-Index Table on the EOS blockchain, where any developer can access the data.

The oracle retrieves the exchange rates from different exchanges every 30 seconds, and then, update the information calling an action on the its smart contract.

The smart contract has three actions:

1. Create new Ticker;
2. Update a Ticker exchange list;
3. Update Ticker rates.

The contract is deployed on ***eosriooracle***.

To access exchange rates, use ***get table eosriooracle eosriooracle ticker***.

If you are developing a smart contract you'll need to include the following structure in your code in order to have access to the data:

        struct exchange {
            string exchange_name;
            double exchange_price;
        };

        TABLE ticker {
            name ticker_name;
            vector<exchange> exchanges;
            double avg_price;
            string timestamp;

            uint64_t primary_key() const {return ticker_name.value;}
        };

        typedef multi_index<"ticker"_n, ticker> ticker_index;

And then, create an instance like this:

        ticker_index tickers("eosriooracle"_n, "eosriooracle"_n.value);   

