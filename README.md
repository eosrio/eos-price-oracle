# EOS Rio Price Oracle

This is a simple service that can store information of different exchange rates in a Multi-Index Table on the EOS blockchain, where any developer can access the data.

The oracle retrieves the exchange rates from different exchanges every 30 seconds, and then, update the information calling an action on the its smart contract.

The smart contract has three actions:

1. Create new Ticker;
2. Update a Ticker exchange list;
3. Update Ticker rates.

The contract is deployed on ***eosriooracle***.

To access exchange rates, use ***get table eosriooracle eosriooracle ticker***.
