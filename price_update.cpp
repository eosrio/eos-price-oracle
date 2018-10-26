#include <eosiolib/eosio.hpp>
#include <eosiolib/name.hpp>
#include <vector>

using namespace eosio;
using namespace std;

CONTRACT price_update : public eosio::contract {
    public:
        using contract::contract;
        price_update(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {}
        
        [[eosio::action]]
        void createticker(name ticker_name, vector<string> exchanges) {
            require_auth(_self);

            ticker_index tickers(_self, _self.value);

            uint64_t ticker_name_index = ticker_name.value;
            auto itr = tickers.find(ticker_name_index);

            eosio_assert(itr == tickers.end(), "Ticker already exists!");

            tickers.emplace(_self, [&](auto& t) {
                t.ticker_name = ticker_name;

                for (int i = 0; i < exchanges.size(); ++i) {
                    exchange ex {exchanges[i], 0.0};
                    t.exchanges.push_back(ex);
                }

                t.avg_price = 0.0;
                t.timestamp = "";
            });
        }

        [[eosio::action]]
        void updateticker(name ticker_name, vector<string> exchanges) {
            require_auth(_self);

            ticker_index tickers(_self, _self.value);

            uint64_t ticker_name_index = ticker_name.value;
            auto itr = tickers.find(ticker_name_index);

            eosio_assert(itr != tickers.end(), "Ticker not found!");

            tickers.modify(itr, _self, [&](auto& t) {
                t.exchanges.clear();

                for (int i = 0; i < exchanges.size(); ++i) {
                    exchange ex {exchanges[i], 0.0};
                    t.exchanges.push_back(ex);
                }
            });

        }

        [[eosio::action]]
        void update(name ticker_name, vector<double> prices, double avg_price, string timestamp) {
            require_auth(_self);

            ticker_index tickers(_self, _self.value);

            uint64_t ticker_name_index = ticker_name.value;
            auto itr = tickers.find(ticker_name_index);

            eosio_assert(itr != tickers.end(), "Ticker not found!");

            eosio_assert(prices.size() <= itr->exchanges.size(), "Wrong values size");

            tickers.modify(itr, _self, [&](auto& p) {
                for (int i = 0; i < prices.size(); ++i) {
                    p.exchanges[i].exchange_price = prices[i];
                }

                p.avg_price = avg_price;
                p.timestamp = timestamp;
            });
        }

    private:
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
};

EOSIO_DISPATCH(price_update, (createticker)(updateticker)(update))
