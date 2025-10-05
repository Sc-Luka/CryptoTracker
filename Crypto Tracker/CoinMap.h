#ifndef COINMAP_H
#define COINMAP_H

#include <QMap>
#include <QString>

class CoinMap {
public:
    static QMap<QString, QString> getNameToIdMap() {
        static QMap<QString, QString> nameToId = {
            {"Bitcoin", "bitcoin"},
            {"Ethereum", "ethereum"},
            {"XRP", "ripple"},
            {"Tether", "tether"},
            {"BNB", "binancecoin"},
            {"Solana", "solana"},
            {"USDC", "usd-coin"},
            {"Lido Staked Ether", "staked-ether"},
            {"Dogecoin", "dogecoin"},
            {"TRON", "tron"},
            {"Cardano", "cardano"},
            {"Wrapped stETH", "wrapped-steth"},
            {"Chainlink", "chainlink"},
            {"Wrapped Beacon ETH", "wrapped-beacon-eth"},
            {"Hyperliquid", "hyperliquid"},
            {"Wrapped Bitcoin", "wrapped-bitcoin"},
            {"Ethena USDe", "ethena-usde"},
            {"Sui", "sui"},
            {"Stellar", "stellar"},
            {"Figure Heloc", "figure-heloc"},
            {"Avalanche", "avalanche-2"},
            {"Wrapped eETH", "wrapped-eeth"},
            {"Bitcoin Cash", "bitcoin-cash"},
            {"WETH", "weth"},
            {"Hedera", "hedera-hashgraph"},
            {"LEO Token", "leo-token"},
            {"Litecoin", "litecoin"},
            {"Cronos", "crypto-com-chain"},
            {"Toncoin", "the-open-network"},
            {"USDS", "usds"},
            {"Shiba Inu", "shiba-inu"},
            {"Binance Bridged USDT", "binance-bridged-usdt-bnb-smart-chain"},
            {"Coinbase Wrapped BTC", "coinbase-wrapped-btc"},
            {"Polkadot", "polkadot"},
            {"WhiteBIT Coin", "whitebit"},
            {"Uniswap", "uniswap"},
            {"Ethena Staked USDe", "ethena-staked-usde"},
            {"World Liberty Financial", "world-liberty-financial"},
            {"Mantle", "mantle"},
            {"Ethena", "ethena"},
            {"Monero", "monero"},
            {"Aave", "aave"},
            {"Bitget Token", "bitget-token"},
            {"Dai", "dai"},
            {"Pepe", "pepe"},
            {"OKB", "okb"},
            {"Bittensor", "bittensor"},
            {"NEAR Protocol", "near"},
            {"Jito Staked SOL", "jito-staked-sol"},
            {"MemeCore", "memecore"}
        };
        return nameToId;
    }
};

#endif // COINMAP_H
