//! Hierarchical tree selection for filtering series and accelerations.
//!
//! This module provides a tree-like selection UI where users can:
//! - Select entire categories with one click
//! - Drill down to specific parameter values
//! - See cross-product of selected items

use std::collections::{HashMap, HashSet};

/// Selection state for a tree node.
#[derive(Debug, Clone, PartialEq)]
pub enum SelectionState {
    /// Nothing selected
    None,
    /// All children selected
    All,
    /// Some children selected (contains IDs of selected children)
    Partial(Vec<String>),
}

impl Default for SelectionState {
    fn default() -> Self {
        SelectionState::None
    }
}

/// A node in the selection tree.
#[derive(Debug, Clone, PartialEq)]
pub struct SelectionNode {
    /// Unique identifier
    pub id: String,
    /// Display label
    pub label: String,
    /// Current selection state
    pub state: SelectionState,
    /// Child nodes (parameters or values)
    pub children: Vec<SelectionNode>,
    /// Whether this node can be expanded
    pub expandable: bool,
    /// Whether currently expanded in UI
    pub expanded: bool,
}

impl SelectionNode {
    /// Create a new selection node.
    pub fn new(id: impl Into<String>, label: impl Into<String>) -> Self {
        Self {
            id: id.into(),
            label: label.into(),
            state: SelectionState::None,
            children: Vec::new(),
            expandable: false,
            expanded: false,
        }
    }

    /// Make this node expandable.
    pub fn with_expandable(mut self, expandable: bool) -> Self {
        self.expandable = expandable;
        self
    }

    /// Add a child node.
    pub fn with_child(mut self, child: SelectionNode) -> Self {
        self.children.push(child);
        self.expandable = true;
        self
    }

    /// Check if this node is selected (all or partial).
    pub fn is_selected(&self) -> bool {
        !matches!(self.state, SelectionState::None)
    }

    /// Get tri-state value for checkbox.
    /// Returns Some(true) if all selected, Some(false) if none, None if partial.
    pub fn tri_state(&self) -> Option<bool> {
        match &self.state {
            SelectionState::None => Some(false),
            SelectionState::All => Some(true),
            SelectionState::Partial(_) => None,
        }
    }

    /// Toggle selection state.
    pub fn toggle(&mut self) {
        self.state = match &self.state {
            SelectionState::None => SelectionState::All,
            SelectionState::All | SelectionState::Partial(_) => SelectionState::None,
        };
        // Propagate to children
        propagate_to_children(&mut self.state, &mut self.children);
    }

    /// Update state from children.
    pub fn update_from_children(&mut self) {
        if self.children.is_empty() {
            return;
        }

        let all_selected = self.children.iter().all(|c| matches!(c.state, SelectionState::All));
        let none_selected = self.children.iter().all(|c| matches!(c.state, SelectionState::None));

        self.state = if all_selected {
            SelectionState::All
        } else if none_selected {
            SelectionState::None
        } else {
            SelectionState::Partial(
                self.children
                    .iter()
                    .filter(|c| c.is_selected())
                    .map(|c| c.id.clone())
                    .collect(),
            )
        };
    }

    /// Count selected leaf nodes.
    pub fn count_selected(&self) -> usize {
        if self.children.is_empty() {
            if self.is_selected() { 1 } else { 0 }
        } else {
            self.children.iter().map(|c| c.count_selected()).sum()
        }
    }

    /// Get all selected leaf paths.
    /// Returns a list of paths from root to each selected leaf.
    pub fn get_selected_paths(&self) -> Vec<Vec<String>> {
        let mut paths = Vec::new();
        self.collect_selected_paths(&mut paths, Vec::new());
        paths
    }

    fn collect_selected_paths(&self, paths: &mut Vec<Vec<String>>, mut current: Vec<String>) {
        current.push(self.id.clone());

        if self.children.is_empty() {
            if self.is_selected() {
                paths.push(current);
            }
        } else {
            for child in &self.children {
                if child.is_selected() {
                    child.collect_selected_paths(paths, current.clone());
                }
            }
        }
    }
}

/// Propagate selection state to children.
fn propagate_to_children(state: &SelectionState, children: &mut [SelectionNode]) {
    for child in children.iter_mut() {
        child.state = state.clone();
        propagate_to_children(state, &mut child.children);
    }
}

/// Build a series selection tree from experiment config.
pub fn build_series_tree(series_instances: &[super::super::config::SeriesInstance]) -> SelectionNode {
    let mut root = SelectionNode::new("series_root", "ALL SERIES")
        .with_expandable(true);
    root.expanded = true;

    // Group series by name
    let mut series_by_name: HashMap<String, Vec<&super::super::config::SeriesInstance>> = HashMap::new();
    for instance in series_instances {
        series_by_name
            .entry(instance.name.clone())
            .or_default()
            .push(instance);
    }

    // Build tree for each series
    for (name, instances) in series_by_name {
        let mut series_node = SelectionNode::new(format!("series_{}", name), &name)
            .with_expandable(true);

        // Collect all parameters and their values
        let mut params: HashMap<String, HashSet<String>> = HashMap::new();
        for inst in &instances {
            for (param_name, value) in &inst.args {
                let value_str = match value {
                    serde_json::Value::Number(n) => n.to_string(),
                    serde_json::Value::String(s) => s.clone(),
                    _ => value.to_string(),
                };
                params.entry(param_name.clone()).or_default().insert(value_str);
            }
        }

        // Build parameter nodes
        for (param_name, values) in params {
            let mut param_node = SelectionNode::new(
                format!("series_{}_{}", name, param_name),
                &param_name,
            )
            .with_expandable(true);

            // Sort values for consistent display
            let mut sorted_values: Vec<_> = values.into_iter().collect();
            sorted_values.sort();

            // Build value nodes
            for value in sorted_values {
                let value_node = SelectionNode::new(
                    format!("series_{}_{}_{}", name, param_name, value),
                    &value,
                );
                param_node.children.push(value_node);
            }

            series_node.children.push(param_node);
        }

        root.children.push(series_node);
    }

    root
}

/// Build an acceleration selection tree from experiment config.
pub fn build_accel_tree(method_instances: &[super::super::config::MethodInstance]) -> SelectionNode {
    let mut root = SelectionNode::new("accel_root", "ALL ACCELERATIONS")
        .with_expandable(true);
    root.expanded = true;

    // Group methods by name
    let mut methods_by_name: HashMap<String, Vec<&super::super::config::MethodInstance>> = HashMap::new();
    for instance in method_instances {
        methods_by_name
            .entry(instance.name.clone())
            .or_default()
            .push(instance);
    }

    // Build tree for each method
    for (name, instances) in methods_by_name {
        let mut method_node = SelectionNode::new(format!("method_{}", name), &name)
            .with_expandable(true);

        // Add n values
        let n_values: HashSet<i64> = instances.iter().map(|i| i.n).collect();
        let mut n_node = SelectionNode::new(format!("method_{}_n", name), "n")
            .with_expandable(true);
        let mut sorted_n: Vec<_> = n_values.into_iter().collect();
        sorted_n.sort();
        for n in sorted_n {
            n_node.children.push(SelectionNode::new(
                format!("method_{}_n_{}", name, n),
                n.to_string(),
            ));
        }
        method_node.children.push(n_node);

        // Add m values
        let m_values: HashSet<i64> = instances.iter().map(|i| i.m).collect();
        let mut m_node = SelectionNode::new(format!("method_{}_m", name), "m")
            .with_expandable(true);
        let mut sorted_m: Vec<_> = m_values.into_iter().collect();
        sorted_m.sort();
        for m in sorted_m {
            m_node.children.push(SelectionNode::new(
                format!("method_{}_m_{}", name, m),
                m.to_string(),
            ));
        }
        method_node.children.push(m_node);

        // Collect additional args
        let mut args: HashMap<String, HashSet<String>> = HashMap::new();
        for inst in &instances {
            for (arg_name, value) in &inst.args {
                let value_str = match value {
                    serde_json::Value::Number(n) => n.to_string(),
                    serde_json::Value::String(s) => s.clone(),
                    _ => value.to_string(),
                };
                args.entry(arg_name.clone()).or_default().insert(value_str);
            }
        }

        // Build arg nodes
        for (arg_name, values) in args {
            let mut arg_node = SelectionNode::new(
                format!("method_{}_{}", name, arg_name),
                &arg_name,
            )
            .with_expandable(true);

            let mut sorted_values: Vec<_> = values.into_iter().collect();
            sorted_values.sort();

            for value in sorted_values {
                arg_node.children.push(SelectionNode::new(
                    format!("method_{}_{}_{}", name, arg_name, value),
                    &value,
                ));
            }

            method_node.children.push(arg_node);
        }

        root.children.push(method_node);
    }

    root
}

/// Build a noise selection tree from experiment config.
pub fn build_noise_tree(noises: &[super::super::config::NoiseDef]) -> SelectionNode {
    let mut root = SelectionNode::new("noise_root", "ALL NOISES")
        .with_expandable(true);
    root.expanded = true;

    for (idx, noise) in noises.iter().enumerate() {
        let label = format!("{} ({})", noise.noise_type, noise.method);
        let noise_node = SelectionNode::new(format!("noise_{}", idx), &label);
        root.children.push(noise_node);
    }

    // Add "No noise" option
    root.children.push(SelectionNode::new("noise_none", "No noise"));

    root
}

/// Build a precision selection tree.
pub fn build_precision_tree(precisions: &[String]) -> SelectionNode {
    let mut root = SelectionNode::new("precision_root", "ALL PRECISIONS")
        .with_expandable(true);
    root.expanded = true;

    for precision in precisions {
        root.children.push(SelectionNode::new(
            format!("precision_{}", precision),
            precision,
        ));
    }

    root
}

/// Selected combination for computation.
#[derive(Debug, Clone)]
pub struct SelectedCombination {
    /// Series name
    pub series_name: String,
    /// Series parameters
    pub series_params: HashMap<String, String>,
    /// Method name
    pub method_name: String,
    /// Method n value
    pub method_n: i64,
    /// Method m value
    pub method_m: i64,
    /// Method additional args
    pub method_args: HashMap<String, String>,
    /// Noise index (None = no noise)
    pub noise_idx: Option<usize>,
    /// Precision
    pub precision: String,
}

/// Generate all selected combinations from trees.
pub fn generate_combinations(
    series_tree: &SelectionNode,
    accel_tree: &SelectionNode,
    noise_tree: &SelectionNode,
    precision_tree: &SelectionNode,
) -> Vec<SelectedCombination> {
    let series_combos = extract_series_combinations(series_tree);
    let accel_combos = extract_accel_combinations(accel_tree);
    let noise_combos = extract_noise_combinations(noise_tree);
    let precisions = extract_precisions(precision_tree);

    let mut combinations = Vec::new();

    for precision in &precisions {
        for series in &series_combos {
            for accel in &accel_combos {
                for noise in &noise_combos {
                    combinations.push(SelectedCombination {
                        series_name: series.0.clone(),
                        series_params: series.1.clone(),
                        method_name: accel.0.clone(),
                        method_n: accel.1,
                        method_m: accel.2,
                        method_args: accel.3.clone(),
                        noise_idx: *noise,
                        precision: precision.clone(),
                    });
                }
            }
        }
    }

    combinations
}

type SeriesCombo = (String, HashMap<String, String>);
type AccelCombo = (String, i64, i64, HashMap<String, String>);

fn extract_series_combinations(tree: &SelectionNode) -> Vec<SeriesCombo> {
    let mut combos = Vec::new();

    for series_node in &tree.children {
        if !series_node.is_selected() {
            continue;
        }

        let name = series_node.label.clone();
        let params = extract_param_values(series_node);
        
        // Generate all parameter combinations
        if params.is_empty() {
            combos.push((name, HashMap::new()));
        } else {
            let param_combos = generate_param_combinations(&params);
            for param_combo in param_combos {
                combos.push((name.clone(), param_combo));
            }
        }
    }

    combos
}

fn extract_accel_combinations(tree: &SelectionNode) -> Vec<AccelCombo> {
    let mut combos = Vec::new();

    for method_node in &tree.children {
        if !method_node.is_selected() {
            continue;
        }

        let name = method_node.label.clone();
        let mut n_values = Vec::new();
        let mut m_values = Vec::new();
        let mut args: HashMap<String, Vec<String>> = HashMap::new();

        for param_node in &method_node.children {
            if !param_node.is_selected() {
                continue;
            }

            match param_node.label.as_str() {
                "n" => {
                    for value_node in &param_node.children {
                        if value_node.is_selected() {
                            if let Ok(n) = value_node.label.parse::<i64>() {
                                n_values.push(n);
                            }
                        }
                    }
                }
                "m" => {
                    for value_node in &param_node.children {
                        if value_node.is_selected() {
                            if let Ok(m) = value_node.label.parse::<i64>() {
                                m_values.push(m);
                            }
                        }
                    }
                }
                _ => {
                    let mut values = Vec::new();
                    for value_node in &param_node.children {
                        if value_node.is_selected() {
                            values.push(value_node.label.clone());
                        }
                    }
                    if !values.is_empty() {
                        args.insert(param_node.label.clone(), values);
                    }
                }
            }
        }

        // Generate combinations
        let n_values = if n_values.is_empty() { vec![33] } else { n_values };
        let m_values = if m_values.is_empty() { vec![4] } else { m_values };

        let arg_combos = if args.is_empty() {
            vec![HashMap::new()]
        } else {
            generate_param_combinations(&args)
        };

        for n in &n_values {
            for m in &m_values {
                for arg_combo in &arg_combos {
                    combos.push((name.clone(), *n, *m, arg_combo.clone()));
                }
            }
        }
    }

    combos
}

fn extract_noise_combinations(tree: &SelectionNode) -> Vec<Option<usize>> {
    let mut combos = Vec::new();

    for (idx, noise_node) in tree.children.iter().enumerate() {
        if noise_node.is_selected() {
            if noise_node.id == "noise_none" {
                combos.push(None);
            } else {
                combos.push(Some(idx));
            }
        }
    }

    combos
}

fn extract_precisions(tree: &SelectionNode) -> Vec<String> {
    let mut precisions = Vec::new();

    for precision_node in &tree.children {
        if precision_node.is_selected() {
            precisions.push(precision_node.label.clone());
        }
    }

    precisions
}

fn extract_param_values(node: &SelectionNode) -> HashMap<String, Vec<String>> {
    let mut params = HashMap::new();

    for param_node in &node.children {
        if !param_node.is_selected() {
            continue;
        }

        let mut values = Vec::new();
        for value_node in &param_node.children {
            if value_node.is_selected() {
                values.push(value_node.label.clone());
            }
        }

        if !values.is_empty() {
            params.insert(param_node.label.clone(), values);
        }
    }

    params
}

fn generate_param_combinations(params: &HashMap<String, Vec<String>>) -> Vec<HashMap<String, String>> {
    if params.is_empty() {
        return vec![HashMap::new()];
    }

    let mut result = vec![HashMap::new()];
    
    for (key, values) in params {
        let mut new_result = Vec::new();
        for existing in &result {
            for value in values {
                let mut new_map = existing.clone();
                new_map.insert(key.clone(), value.clone());
                new_result.push(new_map);
            }
        }
        result = new_result;
    }

    result
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_selection_node_toggle() {
        let mut node = SelectionNode::new("test", "Test")
            .with_child(SelectionNode::new("child1", "Child 1"))
            .with_child(SelectionNode::new("child2", "Child 2"));

        assert_eq!(node.state, SelectionState::None);
        
        node.toggle();
        assert_eq!(node.state, SelectionState::All);
        assert_eq!(node.children[0].state, SelectionState::All);
        assert_eq!(node.children[1].state, SelectionState::All);
        
        node.toggle();
        assert_eq!(node.state, SelectionState::None);
        assert_eq!(node.children[0].state, SelectionState::None);
        assert_eq!(node.children[1].state, SelectionState::None);
    }

    #[test]
    fn test_update_from_children() {
        let mut node = SelectionNode::new("test", "Test")
            .with_child(SelectionNode::new("child1", "Child 1"))
            .with_child(SelectionNode::new("child2", "Child 2"));

        node.children[0].toggle();
        node.update_from_children();
        
        assert!(matches!(node.state, SelectionState::Partial(_)));
    }

    #[test]
    fn test_count_selected() {
        let mut node = SelectionNode::new("test", "Test")
            .with_child(SelectionNode::new("child1", "Child 1"))
            .with_child(SelectionNode::new("child2", "Child 2"));

        assert_eq!(node.count_selected(), 0);
        
        node.children[0].toggle();
        assert_eq!(node.count_selected(), 1);
        
        node.toggle();
        assert_eq!(node.count_selected(), 2);
    }
}
