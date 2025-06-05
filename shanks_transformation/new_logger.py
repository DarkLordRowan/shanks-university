# ============================================================================
# Logging Module
#
# Description:
#   Three-Contour Logging for optimization and sequence transformation tasks.
#   Provides debug, timing, and stats logging with JSON output for structured data.
#   Supports Shanks transformation with specific logging functionality.
#
# Author:
#       - Tsoy Konstantin (kostya_tsoy_03@mail.ru)
#
# Date: 2025-02-20
#
# Notes:
#   - This module is designed to be compatible with optimization frameworks.
#   - JSON logs are structured for easy parsing and analysis.
#   - Ensure all dependencies are installed (e.g., pythonjsonlogger).
# ============================================================================
"""
=== Three-Contour Logging System ===
This module implements a three-contour logging system:
1. Debug Logger: Human-readable logs for debugging (console and file).
2. Timing Logger: Measures execution time of functions and code blocks.
3. Stats Logger: Structured JSON logs for statistical analysis.

=== Usage Instructions ===
1. Initialize loggers:
       from new_logger import setup_loggers
       loggers = setup_loggers(stats_file_path="my_stats.log", time_format="%H:%M:%S.%f")

2. Decorate functions for logging:
       from new_logger import log_execution, timed_algorithm
       @log_execution(logger_name="debug_logger", level=logging.INFO)
       @timed_algorithm(logger_name="timing_logger", level=logging.INFO)
       def solve_problem():
           # Your optimization code here

3. Use context managers:
       from new_logger import Timer, StatsContext
       with Timer("Optimization block"):
           # Code block to time
           ...
       with StatsContext("Training", epochs=100, lr=0.01):
           # Collect statistics
           ...

4. Log Shanks transformation data:
       from new_logger import log_shanks_sequence
       log_shanks_sequence(iteration=1, sequence_value=1.5, transformed_value=1.644, error=0.001)

5. Parse logs:
       from new_logger import LogParser
       parser = LogParser("path/to/log.json")
       shanks_data = parser.get_shanks_sequence()

=== Dependencies ===
- Standard library: os, logging, sys, time, json, re, datetime, functools
- External: pythonjsonlogger (`pip install python-json-logger`)

=== Notes ===
- If a specific function from your project (e.g., framework/logger) is missing, import it below.
- The module supports integration with optimization frameworks (e.g., https://gitlab.com/akulakov111/optimization_7term).
"""
import os
import logging
import sys
import time
import json
import re
from datetime import datetime
from pythonjsonlogger import jsonlogger
import functools

# TODO: Import the missing function here, e.g.:
# from your_project_module import missing_function
# Example: from framework.logger import log_custom_metric

# ----------------------------------------------------------------------------- 
# Custom Formatters
# -----------------------------------------------------------------------------
# The following formatters extend standard logging to support microsecond precision
# in timestamps, which is critical for high-resolution timing in optimization tasks.

class CustomFormatter(logging.Formatter):
    """
    Custom formatter for console logging with microsecond precision.
    Overrides formatTime to use datetime.strftime for %f support.
    """
    def formatTime(self, record, datefmt=None):
        ct = datetime.fromtimestamp(record.created)
        if datefmt:
            s = ct.strftime(datefmt)
        else:
            s = ct.isoformat()
        return s

class CustomJsonFormatter(jsonlogger.JsonFormatter):
    """
    Custom JSON formatter for file logging with microsecond precision.
    Overrides formatTime to use datetime.strftime for %f support.
    """
    def formatTime(self, record, datefmt=None):
        ct = datetime.fromtimestamp(record.created)
        if datefmt:
            s = ct.strftime(datefmt)
        else:
            s = ct.isoformat()
        return s

# ----------------------------------------------------------------------------- 
# Logger Setup
# -----------------------------------------------------------------------------
# The setup_loggers function configures three loggers:
# - debug_logger: For human-readable debug output
# - timing_logger: For timing measurements
# - stats_logger: For JSON-structured statistical data

def setup_loggers(
    debug_level=logging.DEBUG,
    timing_level=logging.DEBUG,
    stats_level=logging.INFO,
    stats_file_path="algorithm_stats.log",
    json_file_path=None,
    time_format="%H:%M:%S"
):
    """
    Initializes three loggers with specified levels and output formats.
    
    Parameters:
        debug_level: Logging level for debug_logger (e.g., logging.DEBUG)
        timing_level: Logging level for timing_logger
        stats_level: Logging level for stats_logger
        stats_file_path: Path to the log file
        json_file_path: Optional separate path for JSON logs
        time_format: Timestamp format (supports %f for microseconds)
    
    Returns:
        Dict with debug_logger, timing_logger, and stats_logger
    """
    stats_file_path = os.getenv("STATS_LOG_FILE", stats_file_path)
    json_path = json_file_path or stats_file_path

    # --- Debug Logger ---
    debug_logger = logging.getLogger("debug_logger")
    debug_logger.setLevel(debug_level)
    debug_logger.handlers.clear()

    debug_formatter = CustomFormatter(
        "%(asctime)s [%(levelname)s] %(name)s: %(message)s",
        datefmt=time_format
    )
    ch_out = logging.StreamHandler(sys.stdout)
    ch_out.setFormatter(debug_formatter)
    debug_logger.addHandler(ch_out)

    ch_file = logging.FileHandler(stats_file_path)
    ch_file.setFormatter(debug_formatter)
    debug_logger.addHandler(ch_file)

    ch_err = logging.StreamHandler(sys.stderr)
    ch_err.setLevel(logging.ERROR)
    ch_err.setFormatter(debug_formatter)
    debug_logger.addHandler(ch_err)

    # --- Timing Logger ---
    timing_logger = logging.getLogger("timing_logger")
    timing_logger.setLevel(timing_level)
    timing_logger.handlers.clear()

    timing_formatter = CustomFormatter(
        "%(asctime)s [TIMING] %(message)s",
        datefmt=time_format
    )
    th_out = logging.StreamHandler(sys.stdout)
    th_out.setFormatter(timing_formatter)
    timing_logger.addHandler(th_out)

    th_file = logging.FileHandler(stats_file_path)
    th_file.setFormatter(timing_formatter)
    timing_logger.addHandler(th_file)

    th_err = logging.StreamHandler(sys.stderr)
    th_err.setLevel(logging.ERROR)
    th_err.setFormatter(timing_formatter)
    timing_logger.addHandler(th_err)

    # --- Stats Logger ---
    stats_logger = logging.getLogger("stats_logger")
    stats_logger.setLevel(stats_level)
    stats_logger.handlers.clear()

    file_formatter = logging.Formatter("%(message)s")
    s_file = logging.FileHandler(json_path)
    s_file.setFormatter(file_formatter)
    stats_logger.addHandler(s_file)

    return {
        "debug_logger": debug_logger,
        "timing_logger": timing_logger,
        "stats_logger": stats_logger
    }

# ----------------------------------------------------------------------------- 
# Decorators for Function Logging and Timing
# -----------------------------------------------------------------------------
# The following decorators provide automatic logging of function entry/exit
# and execution time. Alternative implementations are commented below.

def log_execution(logger_name="debug_logger", level=logging.DEBUG):
    def decorator_func(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            log = logging.getLogger(logger_name)
            func_name = getattr(func, '__name__', repr(func))
            log.log(level, f"Entering function {func_name} args={args}, kwargs={kwargs}")
            result = func(*args, **kwargs)
            log.log(level, f"Exiting function {func_name} result={result}")
            return result
        return wrapper
    return decorator_func

def timed_algorithm(logger_name="timing_logger", level=logging.DEBUG):
    def decorator_func(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            log = logging.getLogger(logger_name)
            func_name = getattr(func, '__name__', repr(func))
            start = time.perf_counter()
            result = func(*args, **kwargs)
            elapsed = time.perf_counter() - start
            log.log(level, f"{func_name} executed in {elapsed:.4f} seconds")
            if isinstance(result, dict):
                result.setdefault('runtime', elapsed)
            return result
        return wrapper
    return decorator_func

# Alternative implementations using 'decorator' library (commented out)
"""
from decorator import decorator

@decorator
def log_execution(func, logger_name="debug_logger", level=logging.DEBUG, *args, **kwargs):
    log = logging.getLogger(logger_name)
    func_name = getattr(func, '__name__', repr(func))
    log.log(level, f"Entering function {func_name} args={args}, kwargs={kwargs}")
    result = func(*args, **kwargs)
    log.log(level, f"Exiting function {func_name} result={result}")
    return result

@decorator
def timed_algorithm(func, logger_name="timing_logger", level=logging.DEBUG, *args, **kwargs):
    log = logging.getLogger(logger_name)
    func_name = getattr(func, '__name__', repr(func))
    start = time.perf_counter()
    result = func(*args, **kwargs)
    elapsed = time.perf_counter() - start
    log.log(level, f"{func_name} executed in {elapsed:.4f} seconds")
    if isinstance(result, dict):
        result.setdefault('runtime', elapsed)
    return result
"""

# ----------------------------------------------------------------------------- 
# Context Managers
# -----------------------------------------------------------------------------
# Context managers for timing and structured statistics logging.

class Timer:
    """
    Context manager for timing code blocks.
    Logs entry and exit with elapsed time via timing_logger.
    """
    def __init__(self, label="Block execution"):
        self.label = label
    def __enter__(self):
        self.start = time.perf_counter()
        logging.getLogger("timing_logger").debug(f"Entering context: {self.label}")
        return self
    def __exit__(self, exc_type, exc_val, exc_tb):
        elapsed = time.perf_counter() - self.start
        logging.getLogger("timing_logger").debug(
            f"Exiting context: {self.label} (elapsed: {elapsed:.4f} seconds)"
        )
        return False

class StatsContext:
    """
    Context manager for structured statistics.
    Logs initial stats on entry and final stats with elapsed time on exit.
    """
    def __init__(self, label="StatsBlock", json_export=True, **initial_stats):
        self.label = label
        self.initial_stats = initial_stats
        self.start_time = time.perf_counter()
    def __enter__(self):
        logging.getLogger("stats_logger").info(json.dumps({
            "type": "STATS_BLOCK_START",
            "label": self.label,
            **self.initial_stats
        }, default=str, separators=(",", ":"), ensure_ascii=False))
        return self
    def __exit__(self, exc_type, exc_val, exc_tb):
        elapsed = time.perf_counter() - self.start_time
        logging.getLogger("stats_logger").info(json.dumps({
            "type": "STATS_BLOCK_END",
            "label": self.label,
            "elapsed": elapsed,
            **self.initial_stats
        }, default=str, separators=(",", ":"), ensure_ascii=False))
        return False

# ----------------------------------------------------------------------------- 
# Logging Functions for Optimization
# -----------------------------------------------------------------------------
# Functions for logging optimization models, iterations, and results.

def log_model(model, logger_name="stats_logger"):
    """
    Logs optimization model details including objective, domain, bounds, and constraints.
    """
    dbg = logging.getLogger("debug_logger")
    stats = logging.getLogger(logger_name)

    cons = getattr(model, "constraints", [])
    if not isinstance(cons, (list, tuple)):
        cons = [cons]

    objective = str(getattr(model, "objective", None))
    domain = getattr(model, "domain", None)
    bounds = getattr(model, "bounds", None)
    constraints = [str(c) for c in cons]

    # Human-readable block
    header = f"{objective}"
    lines = [ header, "{" ]
    lines.append(f"    objective = {objective},")
    lines.append(f"    domain    = {domain},")
    lines.append(f"    bounds    = {bounds},")
    lines.append("    constraints = ")
    lines.append("    {")
    for c in constraints:
        lines.append(f"        {c}")
    lines.append("    }")
    lines.append("}")
    dbg.info("\n" + "\n".join(lines))

    # JSON skeleton
    payload = {
        "type": "OPTIMIZATION_TASK",
        "objective": objective,
        "domain": domain,
        "bounds": bounds,
        "constraints": constraints
    }
    stats.info(json.dumps(
        payload,
        default=str,
        separators=(",", ":"),
        ensure_ascii=False
    ))

def log_iteration_info(
    iteration=None,
    objective_value=None,
    gradient_norm=None,
    constraint_violation=None,
    step_size=None,
    penalty_parameter=None,
    trust_region_radius=None,
    primal_feasibility=None,
    dual_feasibility=None,
    mu=None,
    n_function_evals=None,
    n_gradient_evals=None,
    mode="full",
    logger_name="stats_logger",
    critical_limits=None,
    **extra_metrics
):
    """
    Logs iteration data for optimization processes.
    Supports critical value warnings and flexible metrics.
    """
    dbg = logging.getLogger("debug_logger")
    stats = logging.getLogger(logger_name)

    data = {
        "iter": iteration,
        "objective": objective_value,
        "||gradient||": gradient_norm,
        "constraint_violation": constraint_violation,
        "step_size": step_size
    }
    if critical_limits:
        for key, limit in critical_limits.items():
            value = data.get(key)
            if value is not None:
                if (isinstance(limit, tuple) and (value < limit[0] or value > limit[1])) or \
                   (isinstance(limit, (int, float)) and value > limit):
                    dbg.warning(f"⚠️ Critical value! {key} = {value} (exceeds limit {limit})")
                    stats.warning(json.dumps({
                        "type": "CRITICAL_VALUE",
                        "metric": key,
                        "value": value,
                        "limit": limit,
                        "iteration": iteration,
                    }))

    if mode == "full":
        data.update({
            "penalty_parameter": penalty_parameter,
            "trust_region_radius": trust_region_radius,
            "primal_feasibility": primal_feasibility,
            "dual_feasibility": dual_feasibility,
            "mu": mu,
            "n_function_evals": n_function_evals,
            "n_gradient_evals": n_gradient_evals
        })
        data.update(extra_metrics)

    # Human-readable block
    header = f"Iteration {iteration}"
    lines = [ header, "{" ]
    for k, v in data.items():
        lines.append(f"    {k}: {v},")
    lines.append("}")
    dbg.info("\n" + "\n".join(lines))

    # JSON skeleton
    payload = {"type": "ITERATION_INFO", **data}
    stats.info(json.dumps(
        payload,
        default=str,
        separators=(",", ":"),
        ensure_ascii=False
    ))

def log_optimization_results(
    success=None,
    status=None,
    rho=None,
    logger_name="stats_logger",
    **extra_metrics
):
    """
    Logs final optimization results with success status and additional metrics.
    """
    dbg = logging.getLogger("debug_logger")
    stats = logging.getLogger(logger_name)

    # Human-readable block
    lines = ["[OPTIMIZATION_RESULTS]", "{"]
    lines.append(f"    success: {success},")
    lines.append(f"    status: {status},")
    for key in ["message", "fun", "jac", "hess", "nfev", "njev", "nhev", "nit", "maxcv"]:
        if key in extra_metrics:
            lines.append(f"    {key}: {extra_metrics[key]},")
    lines.append(f"    rho: {rho},")
    for k, v in extra_metrics.items():
        if k not in ["message", "fun", "jac", "hess", "nfev", "njev", "nhev", "nit", "maxcv"]:
            lines.append(f"    {k}: {v},")
    lines.append("}")
    dbg.info("\n" + "\n".join(lines))

    # JSON skeleton
    payload = {"type": "OPTIMIZATION_RESULTS"}
    payload["success"] = success
    payload["status"] = status
    for key in ["message", "fun", "jac", "hess", "nfev", "njev", "nhev", "nit", "maxcv"]:
        if key in extra_metrics:
            payload[key] = extra_metrics[key]
    payload["rho"] = rho
    for k, v in extra_metrics.items():
        if k not in ["message", "fun", "jac", "hess", "nfev", "njev", "nhev", "nit", "maxcv"]:
            payload[k] = v
    stats.info(json.dumps(
        payload,
        default=str,
        separators=(",", ":"),
        ensure_ascii=False
    ))

def log_shanks_sequence(iteration, sequence_value, transformed_value, expected_limit, 
                       prev_error=None, **metrics):
    current_error = abs(transformed_value - expected_limit)
    is_diverging = prev_error and (current_error > prev_error)
    
    data = {
        "type": "SHANKS_SEQUENCE",
        "iter": iteration,
        "S_{iteration}": sequence_value,
        "T_{iteration}": transformed_value,
        "error": current_error,
        "limit": expected_limit,
        "is_diverging": is_diverging,
        "timestamp": datetime.now().isoformat()
    }
    data.update(metrics)
    
    # Логирование в JSON
    stats_logger.info(json.dumps(data))
    
    # Визуальное предупреждение при расходимости
    if is_diverging:
        debug_logger.warning(
            f"⚠️ DIVERGENCE DETECTED! Iter {iteration}: "
            f"Error increased from {prev_error:.2e} to {current_error:.2e}"
        )
        
        # Дополнительная диагностика
        debug_logger.debug(f"Diagnostics - S_n: {sequence_value}, T_n: {transformed_value}, Limit: {expected_limit}")
    return is_diverging


def log_function_2_iteration_data(iteration, a_n_value, t_n_value, **metrics):
    difference = None
    if a_n_value is not None and t_n_value is not None:
        try:
            difference = t_n_value - a_n_value
        except TypeError:
            debug_logger.warning(f"Func2 Iter {iteration}: Could not calculate difference for a_n={a_n_value}, t_n={t_n_value}")

    log_entry = {
        "type": "FUNCTION_2_ANALYSIS",
        "iteration": iteration,
        "a_n_value": a_n_value,
        "t_n_value": t_n_value,
        "difference_t_n_minus_a_n": difference,
        "timestamp": datetime.now().isoformat()
    }
    log_entry.update(metrics)
    stats_logger.info(json.dumps(log_entry, default=str, separators=(",", ":"), ensure_ascii=False))

    debug_logger.info(
        f"Func2 Iter {iteration}: a_n={a_n_value}, t_n={t_n_value}, diff(t_n-a_n)={difference}"
    )

def log_function_3_remainder_data(iteration, remainder_value, **metrics):
    log_entry = {
        "type": "FUNCTION_3_REMAINDER",
        "iteration": iteration,
        "remainder_S_n_minus_T_n": remainder_value,
        "timestamp": datetime.now().isoformat()
    }
    log_entry.update(metrics)
    stats_logger.info(json.dumps(log_entry, default=str, separators=(",", ":"), ensure_ascii=False))

    debug_logger.info(
        f"Func3 Iter {iteration}: Remainder (S_n - T_n) = {remainder_value}"
    )


def log_function_4_comparison_data(iteration, value1, value2, transform1_name="Transform1", transform2_name="Transform2", **metrics):
    comparison_note = "Values are equal or non-comparable."
    if value1 is not None and value2 is not None:
        try:
            if value1 < value2:
                comparison_note = f"{transform1_name} is smaller."
            elif value2 < value1:
                comparison_note = f"{transform2_name} is smaller."
        except TypeError:
            comparison_note = "Cannot compare values due to type mismatch."


    log_entry = {
        "type": "FUNCTION_4_COMPARISON",
        "iteration": iteration,
        transform1_name: value1,
        transform2_name: value2,
        "timestamp": datetime.now().isoformat()
    }
    log_entry.update(metrics)
    stats_logger.info(json.dumps(log_entry, default=str, separators=(",", ":"), ensure_ascii=False))

    debug_logger.info(
        f"Func4 Iter {iteration}: {transform1_name}={value1}, {transform2_name}={value2}. Note: {comparison_note}"
    )

def log_function_5_speed_result(transformation_name, execution_time_ms, n_terms=None, series_x=None, order=None, **metrics):
    log_entry = {
        "type": "FUNCTION_5_SPEED_TEST",
        "transformation_name": transformation_name,
        "execution_time_ms": execution_time_ms,
        "n_terms": n_terms,
        "series_x_value": series_x,
        "order": order,
        "timestamp": datetime.now().isoformat()
    }
    log_entry.update(metrics)
    stats_logger.info(json.dumps(log_entry, default=str, separators=(",", ":"), ensure_ascii=False))

    param_str = []
    if n_terms is not None: param_str.append(f"N={n_terms}")
    if series_x is not None: param_str.append(f"x={series_x}")
    if order is not None: param_str.append(f"Order={order}")
    
    debug_logger.info(
        f"Func5 Speed Test: {transformation_name} ({', '.join(param_str)}) executed in {execution_time_ms} ms"
    )

def log_function_6_multi_transform_data(iteration, s_n_value, transformed_values_dict, expected_limit, prev_transformed_errors_dict=None, **metrics):
    if prev_transformed_errors_dict is None:
        prev_transformed_errors_dict = {}

    base_log_entry = {
        "type": "FUNCTION_6_MULTI_ALGO_ITERATION",
        "iteration": iteration,
        "s_n_value": s_n_value,
        "expected_limit": expected_limit,
        "timestamp": datetime.now().isoformat()
    }
    
    algo_perf = {}
    debug_messages = [
        f"Func6 Iter {iteration}: S_n={s_n_value}, Expected Limit={expected_limit}"
    ]

    for algo_name, t_n_val in transformed_values_dict.items():
        current_error = None
        is_diverging = None
        prev_error = prev_transformed_errors_dict.get(algo_name)

        if t_n_val is not None and expected_limit is not None:
            try:
                current_error = abs(t_n_val - expected_limit)
                if prev_error is not None and current_error is not None:
                    is_diverging = current_error > prev_error
            except TypeError:
                debug_logger.warning(f"Func6 Iter {iteration}, Algo {algo_name}: Could not calculate error for t_n={t_n_val}, limit={expected_limit}")
        
        algo_perf[f"{algo_name}_t_n_value"] = t_n_val
        algo_perf[f"{algo_name}_error"] = current_error
        algo_perf[f"{algo_name}_is_diverging"] = is_diverging
        debug_msg_algo = f"{algo_name}: T_n={t_n_val}, Error={current_error}"
        if is_diverging:
            debug_msg_algo += f"\033[91m⚠️ DIVERGENCE! Error increased from {prev_error} to {current_error}\033[0m"
        debug_messages.append(debug_msg_algo)
    full_log_entry = {}
    full_log_entry.update(base_log_entry)
    full_log_entry.update(algo_perf)
    stats_logger.info(json.dumps(full_log_entry, default=str, separators=(",", ":"), ensure_ascii=False))
    
    for msg in debug_messages:
        debug_logger.info(msg)

# ----------------------------------------------------------------------------- 
# Log Parsing
# -----------------------------------------------------------------------------
# The LogParser class provides an interface for extracting and filtering JSON log entries.

class LogParser:
    """
    Universal log parser for JSON log files.
    Extracts entries and supports filtering by type or key.
    """
    def __init__(self, file_path):
        self.file_path = file_path
        self.entries = []
        self._load()

    def _load(self):
        """
        Reads the log file and parses JSON entries.
        Handles malformed lines by extracting JSON fragments.
        """
        with open(self.file_path, 'r') as f:
            for line in f:
                line = line.strip()
                try:
                    entry = json.loads(line)
                except json.JSONDecodeError:
                    m = re.search(r'(\{.*\})', line)
                    if m:
                        try:
                            entry = json.loads(m.group(1))
                        except json.JSONDecodeError:
                            continue
                    else:
                        continue
                self.entries.append(entry)

    def get_entries(self, entry_type=None):
        """
        Returns all entries or those matching the specified type.
        """
        if entry_type:
            return [e for e in self.entries if e.get('type') == entry_type]
        return list(self.entries)

    def get_model_data(self):
        """Returns entries of type 'OPTIMIZATION_TASK'."""
        return self.get_entries('OPTIMIZATION_TASK')

    def get_iteration_info(self):
        """Returns entries of type 'ITERATION_INFO'."""
        return self.get_entries('ITERATION_INFO')

    def get_results(self):
        """Returns entries of type 'OPTIMIZATION_RESULTS'."""
        return self.get_entries('OPTIMIZATION_RESULTS')

    def get_all_statistics(self):
        """
        Returns entries of types 'ITERATION_INFO', 'OPTIMIZATION_RESULTS', or 'SHANKS_SEQUENCE'.
        """
        stats_types = {'ITERATION_INFO', 'OPTIMIZATION_RESULTS', 'SHANKS_SEQUENCE'}
        return [e for e in self.entries if e.get('type') in stats_types]

    def get_unique_keys(self):
        """
        Returns a sorted list of unique 'key' values in entries.
        """
        return sorted({e.get('key') for e in self.entries if 'key' in e})

    def get_entries_by_key(self, key, entry_type=None):
        """
        Returns entries matching a specific 'key' and optional type.
        """
        return [e for e in self.entries
                if e.get('key') == key and (entry_type is None or e.get('type') == entry_type)]

    def get_model_data_by_key(self, key):
        """Returns 'OPTIMIZATION_TASK' entries for a given key."""
        return self.get_entries_by_key(key, 'OPTIMIZATION_TASK')

    def get_iteration_info_by_key(self, key):
        """Returns 'ITERATION_INFO' entries for a given key."""
        return self.get_entries_by_key(key, 'ITERATION_INFO')

    def get_results_by_key(self, key):
        """Returns 'OPTIMIZATION_RESULTS' entries for a given key."""
        return self.get_entries_by_key(key, 'OPTIMIZATION_RESULTS')

    def get_critical_events(self):
        """Returns entries of type 'CRITICAL_VALUE'."""
        return self.get_entries('CRITICAL_VALUE')

    def get_critical_events_by_key(self, key):
        """Returns critical events for a specific key."""
        return self.get_entries_by_key(key, 'CRITICAL_VALUE')

    def get_shanks_sequence(self):
        """Returns entries of type 'SHANKS_SEQUENCE'."""
        return self.get_entries('SHANKS_SEQUENCE')

    def get_shanks_sequence_by_key(self, key):
        """Returns 'SHANKS_SEQUENCE' entries for a given key."""
        return self.get_entries_by_key(key, 'SHANKS_SEQUENCE')
debug_logger = logging.getLogger("debug_logger")
stats_logger = logging.getLogger("stats_logger")

def log_selected_params_new(selected_series, transformations, functions, x, n, order):
    params_data = {
        "event": "selected_params",
        "selected_series": selected_series,
        "transformations": transformations,
        "functions": functions,
        "x": x,
        "n": n,
        "order": order
    }
    debug_logger.info(json.dumps(params_data))
    try:
        stats_logger.info(json.dumps(params_data))
    except Exception:
        pass

def log_iteration_info(iteration, objective_value, gradient, step, constraints, status=None, **metrics):
    log_data = {
        "event": "iteration",
        "iteration": iteration,
        "objective_value": objective_value,
        "gradient": gradient,
        "step": step,
        "constraints": constraints,
        "status": status
    }
    if metrics:
        log_data.update(metrics)
    debug_logger.info(json.dumps(log_data))

def log_optimization_results(final_obj_value, total_iterations, success=True, termination_status="converged",
                              final_gradient=None, constraints_status=None, **metrics):
    result_data = {
        "event": "optimization_result",
        "final_objective": final_obj_value,
        "iterations": total_iterations,
        "success": success,
        "termination_status": termination_status
    }
    if final_gradient is not None:
        result_data["final_gradient"] = final_gradient
    if constraints_status is not None:
        result_data["constraints_status"] = constraints_status
    if metrics:
        result_data.update(metrics)
    debug_logger.info(json.dumps(result_data))