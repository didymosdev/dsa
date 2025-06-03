#!/bin/bash
set -e

cd "$(dirname "$0")/.."

echo "🐍 Creating Python venv in $(pwd)/venv..."
python3 -m venv venv
source venv/bin/activate

echo "📦 Installing requirements from leetcode/requirements.txt..."
pip install --upgrade pip
pip install -r leetcode/requirements.txt

echo "✅ Setup complete! To activate: source venv/bin/activate in $(pwd)"
