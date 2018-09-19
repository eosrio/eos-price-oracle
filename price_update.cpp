#include <eosiolib/eosio.hpp>

using namespace eosio;
using namespace std;

class price_update : public contract {
    public:
        price_update(account_name self) : contract(self) {}
        
        /// @abi action
        void createticker(name ticker_name, vector<string> exchanges) {
            ticker_index tickers(_self, _self);

            auto itr = tickers.find(ticker_name);
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

        /// @abi action
        void updateticker(name ticker_name, vector<string> exchanges) {
            ticker_index tickers(_self, _self);

            auto itr = tickers.find(ticker_name);
            eosio_assert(itr != tickers.end(), "Ticker not found!");

            tickers.modify(itr, _self, [&](auto& t) {
                t.exchanges.clear();

                for (int i = 0; i < exchanges.size(); ++i) {
                    exchange ex {exchanges[i], 0.0};
                    t.exchanges.push_back(ex);
                }
            });

        }

        /// @abi action
        void update(name ticker_name, vector<double> prices, double avg_price, string timestamp) {
            ticker_index tickers(_self, _self);

            auto itr = tickers.find(ticker_name);
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

        /// @abi table
        struct ticker {
            name ticker_name;
            vector<exchange> exchanges;
            double avg_price;
            string timestamp;

            auto primary_key() const {return ticker_name;}
        };

        typedef multi_index<N(ticker), ticker> ticker_index;
};

EOSIO_ABI(price_update, (createticker)(updateticker)(update))
