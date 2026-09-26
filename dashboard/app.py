"""
MiniOS Dashboard — Streamlit visualization for the C++ OS kernel simulator.

Reads minios_report.json and presents interactive charts, tables, and metrics
for CPU scheduling, page replacement, disk scheduling, and Banker's algorithm.
"""

import json
import subprocess
from pathlib import Path

import streamlit as st

# ---------------------------------------------------------------------------
# Page configuration
# ---------------------------------------------------------------------------

st.set_page_config(
    page_title="MiniOS Dashboard",
    page_icon="",
    layout="wide",
    initial_sidebar_state="expanded",
)

# ---------------------------------------------------------------------------
# Report data wrapper (OOP abstraction over the raw JSON)
# ---------------------------------------------------------------------------


class Report:
    """Typed accessor around the MiniOS simulation report."""

    def __init__(self, data: dict):
        self.data = data

    # -- CPU scheduling -----------------------------------------------------

    def scheduling_algorithms(self) -> list[str]:
        return ["FCFS", "SJF", "Priority", "Round Robin"]

    def get_scheduling(self, algo: str) -> dict:
        return self.data.get(f"scheduling_{algo}", {})

    def get_scheduling_processes(self, algo: str) -> list[dict]:
        return self.get_scheduling(algo).get("processes", [])

    # -- Page replacement ---------------------------------------------------

    def paging_algorithms(self) -> list[str]:
        return ["FIFO", "LRU", "Optimal"]

    def get_paging(self, algo: str) -> dict:
        return self.data.get(f"paging_{algo}", {})

    def get_paging_comparison(self) -> dict:
        rows = {"Algorithm": [], "Hits": [], "Faults": [], "Fault Rate": []}
        for algo in self.paging_algorithms():
            entry = self.get_paging(algo)
            if not entry:
                continue
            total = entry["hits"] + entry["faults"]
            rate = (100 * entry["faults"] / total) if total else 0.0
            rows["Algorithm"].append(algo)
            rows["Hits"].append(entry["hits"])
            rows["Faults"].append(entry["faults"])
            rows["Fault Rate"].append(f"{rate:.1f}%")
        return rows

    # -- Disk scheduling ----------------------------------------------------

    def disk_algorithms(self) -> list[str]:
        return ["FCFS", "SSTF", "SCAN", "C-SCAN"]

    def get_disk(self, algo: str) -> dict:
        return self.data.get(f"disk_{algo}", {})

    def get_disk_comparison(self) -> dict:
        rows = {"Algorithm": [], "Total Seek": [], "Final Head": []}
        for algo in self.disk_algorithms():
            entry = self.get_disk(algo)
            if not entry:
                continue
            rows["Algorithm"].append(algo)
            rows["Total Seek"].append(entry["totalSeek"])
            rows["Final Head"].append(entry["headPosition"])
        return rows

    # -- Banker's algorithm -------------------------------------------------

    def get_banker(self) -> dict:
        return self.data.get("banker", {})


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def find_report() -> Path | None:
    """Search for minios_report.json in common locations."""
    candidates = [
        Path("minios_report.json"),
        Path("../minios_report.json"),
        Path("build/minios_report.json"),
        Path("../build/minios_report.json"),
    ]
    for path in candidates:
        if path.exists():
            return path
    return None


def load_report(path: Path) -> Report:
    with open(path) as f:
        return Report(json.load(f))


def find_executable() -> Path | None:
    """Locate the compiled MiniOS executable."""
    candidates = [
        Path("build/MiniOS.exe"),
        Path("../build/MiniOS.exe"),
    ]
    for exe in candidates:
        if exe.exists():
            return exe
    return None


# ---------------------------------------------------------------------------
# Sidebar
# ---------------------------------------------------------------------------

st.sidebar.title("MiniOS Simulator")
st.sidebar.markdown("C++ OS Kernel Visualization Dashboard")
st.sidebar.markdown("---")

report_path = find_report()
if report_path:
    st.sidebar.success(f"Report loaded: `{report_path}`")
else:
    st.sidebar.warning("No report found")

if st.sidebar.button("Run Simulator", type="primary"):
    exe = find_executable()
    if exe:
        with st.spinner("Running simulation..."):
            subprocess.run([str(exe)], capture_output=True, text=True)
        st.sidebar.success("Simulation complete!")
        st.rerun()
    else:
        st.sidebar.error("MiniOS.exe not found — build the C++ project first.")

st.sidebar.markdown("---")
st.sidebar.markdown("### About")
st.sidebar.markdown(
    "MiniOS is a C++ operating system simulator built with OOP principles. "
    "This dashboard visualizes simulation results from `minios_report.json`."
)

# ---------------------------------------------------------------------------
# Main content
# ---------------------------------------------------------------------------

st.title("MiniOS — C++ OS Kernel Simulator")
st.markdown(
    "Interactive visualization dashboard for the MiniOS operating system simulator. "
    "The simulator demonstrates **CPU scheduling**, **paging**, **disk scheduling**, "
    "and **Banker's algorithm** — all built with OOP principles in C++."
)

if report_path is None:
    st.info(
        "No simulation report found. Click **Run Simulator** in the sidebar to generate one."
    )
    st.stop()

report = load_report(report_path)

# ---------------------------------------------------------------------------
# Tabs
# ---------------------------------------------------------------------------

tab_overview, tab_cpu, tab_paging, tab_disk, tab_banker = st.tabs(
    [
        "Overview",
        "CPU Scheduling",
        "Page Replacement",
        "Disk Scheduling",
        "Banker's Algorithm",
    ]
)

# ---------------------------------------------------------------------------
# Overview tab
# ---------------------------------------------------------------------------

with tab_overview:
    st.header("System Overview")

    col1, col2, col3, col4 = st.columns(4)
    col1.metric("CPU Schedulers", "4", "FCFS, SJF, Priority, RR")
    col2.metric("Page Replacement", "3", "FIFO, LRU, Optimal")
    col3.metric("Disk Schedulers", "4", "FCFS, SSTF, SCAN, C-SCAN")
    col4.metric("Banker's Algorithm", "Active", "Deadlock avoidance")

    st.markdown("---")

    st.subheader("Simulation Summary")
    summary = {
        "Subsystem": [
            "CPU Scheduling",
            "Page Replacement",
            "Disk Scheduling",
            "Banker's Algorithm",
        ],
        "Algorithms": [4, 3, 4, 1],
        "Status": ["Complete", "Complete", "Complete", "Complete"],
    }
    st.dataframe(summary, use_container_width=True, hide_index=True)

    st.markdown("---")
    st.markdown(
        "This dashboard reads `minios_report.json` generated by the C++ simulator. "
        "Each tab shows detailed results for a specific subsystem."
    )

# ---------------------------------------------------------------------------
# CPU Scheduling tab
# ---------------------------------------------------------------------------

with tab_cpu:
    st.header("CPU Scheduling Algorithms")
    st.markdown("Process states after scheduling with each algorithm.")

    algo = st.selectbox(
        "Select Algorithm",
        report.scheduling_algorithms(),
        key="cpu_algo",
    )

    processes = report.get_scheduling_processes(algo)

    if processes:
        col1, col2, col3 = st.columns(3)
        col1.metric("Total Processes", len(processes))
        col2.metric("Total CPU Time", sum(p["cpu"] for p in processes))
        col3.metric("Total Instructions", sum(p["pc"] for p in processes))

        st.subheader("Process Table")
        table = {
            "PID": [p["pid"] for p in processes],
            "Name": [p["name"] for p in processes],
            "Priority": [p["priority"] for p in processes],
            "State": [p["state"] for p in processes],
            "CPU Time": [p["cpu"] for p in processes],
            "Program Counter": [p["pc"] for p in processes],
        }
        st.dataframe(table, use_container_width=True, hide_index=True)

        st.subheader("CPU Time Distribution")
        chart = {p["name"]: p["cpu"] for p in processes}
        st.bar_chart(chart)
    else:
        st.warning(f"No data for {algo}.")

# ---------------------------------------------------------------------------
# Page Replacement tab
# ---------------------------------------------------------------------------

with tab_paging:
    st.header("Page Replacement Algorithms")
    st.markdown("Frame allocation and hit/fault statistics for each algorithm.")

    st.subheader("Algorithm Comparison")
    st.dataframe(report.get_paging_comparison(), use_container_width=True, hide_index=True)

    st.subheader("Hits vs Faults")
    chart = {
        "Hits": [
            report.get_paging(a).get("hits", 0) for a in report.paging_algorithms()
        ],
        "Faults": [
            report.get_paging(a).get("faults", 0) for a in report.paging_algorithms()
        ],
    }
    st.bar_chart(chart)

    st.markdown("---")

    algo = st.selectbox(
        "Select Algorithm for Detailed View",
        report.paging_algorithms(),
        key="paging_algo",
    )

    entry = report.get_paging(algo)
    if entry:
        st.subheader(f"{algo} — Frame Allocation")

        total = entry["hits"] + entry["faults"]
        rate = (100 * entry["faults"] / total) if total else 0.0

        col1, col2, col3 = st.columns(3)
        col1.metric("Hits", entry["hits"])
        col2.metric("Faults", entry["faults"])
        col3.metric("Fault Rate", f"{rate:.1f}%")

        frames = entry.get("frames", [])
        if frames:
            frame_table = {
                "Frame": [f["frame"] for f in frames],
                "Occupied": ["Yes" if f["occupied"] else "No" for f in frames],
                "PID": [f.get("pid", "-") if f["occupied"] else "-" for f in frames],
                "Page": [f.get("page", "-") if f["occupied"] else "-" for f in frames],
            }
            st.dataframe(frame_table, use_container_width=True, hide_index=True)
    else:
        st.warning(f"No data for {algo}.")

# ---------------------------------------------------------------------------
# Disk Scheduling tab
# ---------------------------------------------------------------------------

with tab_disk:
    st.header("Disk Scheduling Algorithms")
    st.markdown("Seek distance comparison for each disk scheduling algorithm.")

    st.subheader("Algorithm Comparison")
    st.dataframe(report.get_disk_comparison(), use_container_width=True, hide_index=True)

    st.subheader("Total Seek Distance")
    seek = {
        algo: report.get_disk(algo).get("totalSeek", 0)
        for algo in report.disk_algorithms()
    }
    st.bar_chart(seek)

    st.markdown("---")

    algo = st.selectbox(
        "Select Algorithm for Detailed View",
        report.disk_algorithms(),
        key="disk_algo",
    )

    entry = report.get_disk(algo)
    if entry:
        st.subheader(f"{algo} — Detailed Statistics")

        col1, col2, col3 = st.columns(3)
        col1.metric("Total Seek", f"{entry['totalSeek']} cylinders")
        col2.metric("Final Head Position", f"{entry['headPosition']}")
        col3.metric("Requests Remaining", len(entry.get("queue", [])))
    else:
        st.warning(f"No data for {algo}.")

# ---------------------------------------------------------------------------
# Banker's Algorithm tab
# ---------------------------------------------------------------------------

with tab_banker:
    st.header("Banker's Algorithm")
    st.markdown("Resource allocation state and deadlock avoidance analysis.")

    banker = report.get_banker()
    if not banker:
        st.warning("No Banker's algorithm data found.")
    else:
        st.subheader("Available Resources")
        avail = banker.get("available", [])
        cols = st.columns(len(avail))
        for i, (col, val) in enumerate(zip(cols, avail)):
            col.metric(f"Resource {chr(65 + i)}", val)

        st.markdown("---")

        st.subheader("Process Resource Tables")
        proc_table = []
        for p in banker.get("processes", []):
            proc_table.append(
                {
                    "PID": f"P{p['pid']}",
                    "Allocation": str(p["allocation"]),
                    "Max": str(p["max"]),
                    "Need": str(p["need"]),
                }
            )
        st.dataframe(proc_table, use_container_width=True, hide_index=True)

        st.markdown("---")

        st.subheader("Resource A Distribution")
        resource_a = {
            "Allocation": [p["allocation"][0] for p in banker.get("processes", [])],
            "Max": [p["max"][0] for p in banker.get("processes", [])],
            "Need": [p["need"][0] for p in banker.get("processes", [])],
        }
        st.bar_chart(resource_a)
