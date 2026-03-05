use indexmap::IndexMap;
use std::collections::HashMap;

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

        let all_selected = self
            .children
            .iter()
            .all(|c| matches!(c.state, SelectionState::All));
        let none_selected = self
            .children
            .iter()
            .all(|c| matches!(c.state, SelectionState::None));

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
pub fn build_series_tree(
    series_instances: &[super::super::experiment::SeriesInstance],
) -> SelectionNode {
    let mut root = SelectionNode::new("series_root", "ALL SERIES").with_expandable(true);
    root.expanded = true;

    // Use IndexMap to preserve order of series and their parameters
    let mut series_nodes: IndexMap<String, SelectionNode> = IndexMap::new();

    for instance in series_instances {
        let series_node = series_nodes
            .entry(instance.name.clone())
            .or_insert_with(|| {
                SelectionNode::new(format!("series_{}", instance.name), instance.name.clone())
                    .with_expandable(true)
            });

        // Use a temporary IndexMap for parameters to handle values consistently
        // Note: instance.args is still HashMap, but we'll collect values in discovery order
        for (param_name, value) in &instance.args {
            let value_str = match value {
                serde_json::Value::Number(n) => n.to_string(),
                serde_json::Value::String(s) => s.clone(),
                _ => value.to_string(),
            };

            // Find or create param node using standard Vec position for simplicity as it's already there
            // or we could use another IndexMap if nested depth was high.
            let param_node = if let Some(node) = series_node
                .children
                .iter_mut()
                .find(|c| c.label == *param_name)
            {
                node
            } else {
                series_node.children.push(
                    SelectionNode::new(
                        format!("{}_{}", series_node.id, param_name),
                        param_name.clone(),
                    )
                    .with_expandable(true),
                );
                series_node.children.last_mut().unwrap()
            };

            // Add value if not present
            if !param_node.children.iter().any(|c| c.label == value_str) {
                param_node.children.push(SelectionNode::new(
                    format!("{}_{}", param_node.id, value_str),
                    value_str,
                ));
            }
        }
    }

    root.children = series_nodes.into_values().collect();
    root
}

/// Build an acceleration selection tree from experiment config.
pub fn build_accel_tree(
    method_instances: &[super::super::experiment::AccelInstance],
) -> SelectionNode {
    let mut root = SelectionNode::new("accel_root", "ALL ACCELERATIONS").with_expandable(true);
    root.expanded = true;

    let mut method_nodes: IndexMap<String, SelectionNode> = IndexMap::new();

    for instance in method_instances {
        let method_node = method_nodes
            .entry(instance.name.clone())
            .or_insert_with(|| {
                SelectionNode::new(format!("method_{}", instance.name), instance.name.clone())
                    .with_expandable(true)
            });

        // Ensure "m" node exists
        if !method_node.children.iter().any(|c| c.label == "m") {
            method_node.children.push(
                SelectionNode::new(format!("{}_m", method_node.id), "m").with_expandable(true),
            );
        }
        let m_node = method_node
            .children
            .iter_mut()
            .find(|c| c.label == "m")
            .unwrap();
        let m_str = instance.m.to_string();
        if !m_node.children.iter().any(|c| c.label == m_str) {
            m_node.children.push(SelectionNode::new(
                format!("{}_{}", m_node.id, m_str),
                m_str,
            ));
        }

        // Process additional args
        for (arg_name, value) in &instance.args {
            let value_str = match value {
                serde_json::Value::Number(n) => n.to_string(),
                serde_json::Value::String(s) => s.clone(),
                _ => value.to_string(),
            };

            let arg_node = if let Some(node) = method_node
                .children
                .iter_mut()
                .find(|c| c.label == *arg_name)
            {
                node
            } else {
                method_node.children.push(
                    SelectionNode::new(
                        format!("{}_{}", method_node.id, arg_name),
                        arg_name.clone(),
                    )
                    .with_expandable(true),
                );
                method_node.children.last_mut().unwrap()
            };

            if !arg_node.children.iter().any(|c| c.label == value_str) {
                arg_node.children.push(SelectionNode::new(
                    format!("{}_{}", arg_node.id, value_str),
                    value_str,
                ));
            }
        }
    }

    root.children = method_nodes.into_values().collect();
    root
}

/// Build a noise selection tree from experiment config.
pub fn build_noise_tree(
    noise_instances: &[super::super::experiment::NoiseInstance],
) -> SelectionNode {
    let mut root = SelectionNode::new("noise_root", "ALL NOISES").with_expandable(true);
    root.expanded = true;

    // Add "No noise" option
    let mut no_noise = SelectionNode::new("noise_none", "No noise");
    no_noise.state = SelectionState::All;
    root.children.push(no_noise);

    let mut type_nodes: IndexMap<String, SelectionNode> = IndexMap::new();

    for instance in noise_instances {
        let type_node = type_nodes
            .entry(instance.noise_type.clone())
            .or_insert_with(|| {
                SelectionNode::new(
                    format!("noise_{}", instance.noise_type),
                    instance.noise_type.clone(),
                )
                .with_expandable(true)
            });

        let method_node = if let Some(node) = type_node
            .children
            .iter_mut()
            .find(|c| c.label == instance.method)
        {
            node
        } else {
            type_node.children.push(
                SelectionNode::new(
                    format!("{}_{}", type_node.id, instance.method),
                    instance.method.clone(),
                )
                .with_expandable(true),
            );
            type_node.children.last_mut().unwrap()
        };

        // Seed node
        if !method_node.children.iter().any(|c| c.label == "seed") {
            method_node.children.push(
                SelectionNode::new(format!("{}_seed", method_node.id), "seed").with_expandable(true),
            );
        }
        let seed_node = method_node
            .children
            .iter_mut()
            .find(|c| c.label == "seed")
            .unwrap();
        let seed_str = instance.seed.to_string();
        if !seed_node.children.iter().any(|c| c.label == seed_str) {
            seed_node.children.push(SelectionNode::new(
                format!("{}_{}", seed_node.id, seed_str),
                seed_str,
            ));
        }

        // Params
        for (param_name, value) in &instance.args {
            let value_str = match value {
                serde_json::Value::Number(n) => n.to_string(),
                serde_json::Value::String(s) => s.clone(),
                _ => value.to_string(),
            };

            let param_node = if let Some(node) = method_node
                .children
                .iter_mut()
                .find(|c| c.label == *param_name)
            {
                node
            } else {
                method_node.children.push(
                    SelectionNode::new(
                        format!("{}_{}", method_node.id, param_name),
                        param_name.clone(),
                    )
                    .with_expandable(true),
                );
                method_node.children.last_mut().unwrap()
            };

            if !param_node.children.iter().any(|c| c.label == value_str) {
                param_node.children.push(SelectionNode::new(
                    format!("{}_{}", param_node.id, value_str),
                    value_str,
                ));
            }
        }
    }

    root.children.extend(type_nodes.into_values());
    root
}

/// Build a precision selection tree.
pub fn build_precision_tree(precisions: &[String]) -> SelectionNode {
    let mut root = SelectionNode::new("precision_root", "ALL PRECISIONS").with_expandable(true);
    root.expanded = true;

    for precision in precisions {
        root.children.push(SelectionNode::new(
            format!("precision_{}", precision),
            precision.clone(),
        ));
    }

    root
}

/// Build a filter selection tree from experiment config.
pub fn build_filter_tree(
    filter_instances: &[super::super::experiment::FilterInstance],
) -> SelectionNode {
    let mut root = SelectionNode::new("filter_root", "ALL FILTERS").with_expandable(true);
    root.expanded = true;

    let mut filter_nodes: IndexMap<String, SelectionNode> = IndexMap::new();

    for instance in filter_instances {
        let filter_node = filter_nodes
            .entry(instance.filter_type.clone())
            .or_insert_with(|| {
                SelectionNode::new(
                    format!("filter_{}", instance.filter_type),
                    instance.filter_type.clone(),
                )
                .with_expandable(true)
            });

        for (param_name, value) in &instance.args {
            let value_str = match value {
                serde_json::Value::Number(n) => n.to_string(),
                serde_json::Value::String(s) => s.clone(),
                _ => value.to_string(),
            };

            let param_node = if let Some(node) = filter_node
                .children
                .iter_mut()
                .find(|c| c.label == *param_name)
            {
                node
            } else {
                filter_node.children.push(
                    SelectionNode::new(
                        format!("{}_{}", filter_node.id, param_name),
                        param_name.clone(),
                    )
                    .with_expandable(true),
                );
                filter_node.children.last_mut().unwrap()
            };

            if !param_node.children.iter().any(|c| c.label == value_str) {
                param_node.children.push(SelectionNode::new(
                    format!("{}_{}", param_node.id, value_str),
                    value_str,
                ));
            }
        }
    }

    root.children.extend(filter_nodes.into_values());
    root
}

/// Structured selection from the UI trees.
#[derive(Debug, Clone, Default)]
pub struct AppSelection {
    pub precisions: Vec<String>,
    pub series: Vec<SeriesCombo>,
    pub noises: Vec<Option<NoiseCombo>>,
    pub accels: Vec<AccelCombo>,
    pub filters: Vec<FilterCombo>,
}

/// Extract all selected values from trees without combinatorial expansion.
pub fn extract_selection(
    series_tree: &SelectionNode,
    accel_tree: &SelectionNode,
    noise_tree: &SelectionNode,
    filter_tree: &SelectionNode,
    precision_tree: &SelectionNode,
) -> AppSelection {
    AppSelection {
        precisions: extract_precisions(precision_tree),
        series: extract_series_combinations(series_tree),
        noises: extract_noise_combinations(noise_tree),
        accels: extract_accel_combinations(accel_tree),
        filters: extract_filter_combinations(filter_tree),
    }
}

type SeriesCombo = (String, HashMap<String, String>);
type AccelCombo = (String, i64, HashMap<String, String>);
type FilterCombo = (Option<String>, HashMap<String, String>);
pub type NoiseCombo = (String, String, HashMap<String, String>, i64);

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
        let mut m_values = Vec::new();
        let mut args: HashMap<String, Vec<String>> = HashMap::new();

        for param_node in &method_node.children {
            if !param_node.is_selected() {
                continue;
            }

            match param_node.label.as_str() {
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

        let m_values = if m_values.is_empty() {
            vec![4]
        } else {
            m_values
        };

        let arg_combos = if args.is_empty() {
            vec![HashMap::new()]
        } else {
            generate_param_combinations(&args)
        };

        for m in &m_values {
            for arg_combo in &arg_combos {
                combos.push((name.clone(), *m, arg_combo.clone()));
            }
        }
    }

    combos
}

fn extract_noise_combinations(tree: &SelectionNode) -> Vec<Option<NoiseCombo>> {
    let mut combos = Vec::new();

    for type_node in &tree.children {
        if !type_node.is_selected() {
            continue;
        }

        if type_node.id == "noise_none" {
            combos.push(None);
            continue;
        }

        let noise_type = type_node.label.clone();

        for method_node in &type_node.children {
            if !method_node.is_selected() {
                continue;
            }

            let method = method_node.label.clone();
            let mut seed_values = Vec::new();
            let mut args: HashMap<String, Vec<String>> = HashMap::new();

            for param_node in &method_node.children {
                if !param_node.is_selected() {
                    continue;
                }

                if param_node.label == "seed" {
                    for value_node in &param_node.children {
                        if value_node.is_selected() {
                            if let Ok(s) = value_node.label.parse::<i64>() {
                                seed_values.push(s);
                            }
                        }
                    }
                } else {
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

            let seed_values = if seed_values.is_empty() {
                vec![0]
            } else {
                seed_values
            };
            let arg_combos = if args.is_empty() {
                vec![HashMap::new()]
            } else {
                generate_param_combinations(&args)
            };

            for seed in &seed_values {
                for arg_combo in &arg_combos {
                    combos.push(Some((
                        noise_type.clone(),
                        method.clone(),
                        arg_combo.clone(),
                        *seed,
                    )));
                }
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

fn extract_filter_combinations(tree: &SelectionNode) -> Vec<FilterCombo> {
    let mut combos = Vec::new();

    combos.push((None, HashMap::new()));
    for filter_node in &tree.children {
        if !filter_node.is_selected() {
            continue;
        }

        let name = filter_node.label.clone();
        let params = extract_param_values(filter_node);

        if params.is_empty() {
            combos.push((Some(name), HashMap::new()));
        } else {
            let param_combos = generate_param_combinations(&params);
            for param_combo in param_combos {
                combos.push((Some(name.clone()), param_combo));
            }
        }
    }

    if combos.is_empty() {
        combos.push((None, HashMap::new()));
    }

    combos
}

fn generate_param_combinations(
    params: &HashMap<String, Vec<String>>,
) -> Vec<HashMap<String, String>> {
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
