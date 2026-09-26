# MiniOS Dashboard

Python Streamlit visualization dashboard for the MiniOS C++ OS kernel simulator.

## Setup

1. Install dependencies:

   ```bash
   pip install -r requirements.txt
   ```

2. Run the dashboard:

   ```bash
   streamlit run app.py
   ```

3. Click **Run Simulator** in the sidebar to generate fresh results.

## Requirements

- The C++ simulator must be built (`build/MiniOS.exe`)
- Python 3.8+
- Streamlit 1.28+

## Features

- **Overview** — system summary and simulation status
- **CPU Scheduling** — process tables and CPU time charts for FCFS, SJF, Priority, Round Robin
- **Page Replacement** — frame allocation, hits/faults comparison for FIFO, LRU, Optimal
- **Disk Scheduling** — seek distance comparison for FCFS, SSTF, SCAN, C-SCAN
- **Banker's Algorithm** — resource allocation tables and distribution charts
