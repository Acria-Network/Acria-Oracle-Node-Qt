![Header](img/github_header_color.svg)

# Acria-Oracle-Node-Qt Client

Acria Oracle Node Client implemented in C++/Qt

![GitHub](https://img.shields.io/github/license/Acria-Network/Acria-Oracle-Node-Qt)
![GitHub last commit](https://img.shields.io/github/last-commit/Acria-Network/Acria-Oracle-Node-Qt)
![GitHub](https://img.shields.io/badge/Qt-v5.15-brightgreen)
![GitHub](https://img.shields.io/badge/OS-Linux%2FMacOS%2FWindows-brightgreen)
![GitHub](https://badgen.net/twitter/follow/acrianetwork)

One of the most pressing issues when developing smart contracts is the lack of real-world data. But due to technical limitations, such as the consensus protocol, no blockchain has been able to solve this major limitation. The Acria Network solves exactly this problem with the help of so-called Oracle Nodes that don't require a middleman. In addition to this, it offers cross-chain support to supply various blockchains with real-world data.

[https://acria.network/](https://https://acria.network/)

## Building

OS X & Linux:

```sh
qmake Acria-Oracle-Node-Qt.pro
make
```

Using the example config and settings file:

```sh
cp configs/settings.conf.example settings.conf
cp configs/config.conf.example config.conf
```

This application is intended to be used with [Acria-Substrate](https://github.com/Acria-Network/acria-substrate) and [Acria-Contracts](https://github.com/Acria-Network/Acria-Contracts). Please follow the instructions on the respective repository.

## Whitepaper / Technical Specification

<a href="https://acria.network/whitepaper" target="_blank"><img src="img/documents-document.svg" width="100" height="100" /></a>

## Aims

The Acria Network has many design goals in order to achieve all its goals.

### Cross-Chain

Every single process has to be optimized to allow cross-chain communication with various different blockchains. On top of this, as many crypto currencies as possible should be use-able as collateral and therefore stake-able.

### Easy to use

By allowing the user to use their preferred cryptocurrency as collateral we completely eliminate the hassle of acquiring the currency used by the network. In addition to this, every end-user program is optimized to be simple and as straightforward as possible to use.

### Extensibility and Scalability

By design, the Acria Network is designed to allow the easy implementation of publicly available data by the initial distributor of that data. This guarantees that the network is always up to date with the needs of its users. On top of this it is able to keep the latency very low and the throughput high.

### Trustworthy

The Acria Network only allows the owner of the data to operate an oracle node. The other users are then allowed to stake their preferred currency on those oracle nodes. Should the operator of the oracle publish incorrect data, some of the stakers gains will be slashed. Therefore the stakers give validity to the data provided by the oracle node as they prefer to only stake on the most trustworthy nodes.

## License

![GitHub](https://img.shields.io/github/license/Acria-Network/Acria-Oracle-Node-Qt)

Distributed under the MIT license. See ``LICENSE`` for more information.


## Meta

Acria Network – [@acrianetwork](https://twitter.com/acrianetwork) – help@acria.network

[Telegram](https://t.me/acria_network) - [Twitter](https://twitter.com/acrianetwork) - [Facebook](https://www.facebook.com/Acria-102161605171826) - [Youtube](https://www.youtube.com/channel/UCCoP1eb6cGd7XTq0sAqP7cg)

[https://github.com/Acria-Network/Acria-Oracle-Node-Qt](https://github.com/Acria-Network/Acria-Oracle-Node-Qt)
