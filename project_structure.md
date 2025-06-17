# SpeedQuant Project Structure

This document outlines the directory structure and key files for the SpeedQuant project.

## Directory Structure

```
speedquant/
├── core/                      # C++ core modules
│   ├── include/               # Header files
│   │   ├── market_data/       # Market data processing headers
│   │   ├── order/             # Order management headers
│   │   ├── risk/              # Risk control headers
│   │   └── utils/             # Utility headers
│   ├── src/                   # Source files
│   │   ├── market_data/       # Market data processing implementation
│   │   ├── order/             # Order management implementation
│   │   ├── risk/              # Risk control implementation
│   │   └── utils/             # Utility implementation
│   └── tests/                 # C++ tests
├── gui/                       # Qt GUI components
│   ├── assets/                # GUI assets (icons, etc.)
│   ├── include/               # GUI headers
│   ├── src/                   # GUI source files
│   └── ui/                    # Qt UI files
├── speedquant_lib/             # Python library modules
│   ├── agents/                # AI agents implementation
│   │   ├── news_parser/       # News parsing agent
│   │   ├── portfolio/         # Portfolio optimization agent
│   │   ├── strategy_gen/      # Strategy generation agent
│   │   └── review/            # Trading review agent
│   ├── api/                   # FastAPI implementation
│   ├── backtest/              # Backtesting system
│   ├── bridge/                # C++/Python bridge
│   ├── data/                  # Data handling utilities
│   ├── models/                # AI models
│   ├── risk/                  # Risk management (Python layer)
│   ├── strategies/            # Trading strategies
│   │   ├── templates/         # Strategy templates
│   │   └── plugins/           # Strategy plugins
│   └── utils/                 # Python utilities
├── data_engine/               # Scrapybara integration
│   ├── crawlers/              # Web crawlers
│   ├── parsers/               # Data parsers
│   └── sentiment/             # Sentiment analysis
├── docs/                      # Documentation
│   ├── api/                   # API documentation
│   ├── architecture/          # Architecture diagrams
│   └── user_guide/            # User guides
├── scripts/                   # Utility scripts
│   ├── build/                 # Build scripts
│   ├── deploy/                # Deployment scripts
│   └── tools/                 # Development tools
├── tests/                     # Integration tests
│   ├── e2e/                   # End-to-end tests
│   ├── integration/           # Integration tests
│   └── performance/           # Performance tests
├── .gitignore                 # Git ignore file
├── CMakeLists.txt             # CMake configuration for C++ components
├── pyproject.toml             # Python project configuration
├── requirements.txt           # Python dependencies
├── setup.py                   # Python package setup
└── README.md                  # Project README
```
