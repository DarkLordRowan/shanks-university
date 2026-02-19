//! Custom tree widget with tri-state checkboxes.
//!
//! This module provides a proper tree view with:
//! - Expand/collapse indicators (▶/▼)
//! - Tri-state checkboxes (☐/☑/▣)
//! - Proper indentation
//! - Click-to-toggle selection

use super::selection::{SelectionNode, SelectionState};

/// Draw a tree node with custom styling.
/// 
/// Visual format:
/// ```text
/// [-] ALL SERIES
///   [-] Series A
///     [-] param1
///       [+] 4
///       [+] 5
///       [ ] 6
///     [-] param2
///       [ ] 6
///       [ ] 7
///       [+] 8
/// ```
pub fn draw_tree(ui: &mut egui::Ui, node: &mut SelectionNode) {
    draw_tree_node(ui, node, 0);
}

fn draw_tree_node(ui: &mut egui::Ui, node: &mut SelectionNode, depth: usize) {
    let indent = depth as f32 * 20.0;
    
    ui.horizontal(|ui| {
        // Add indentation
        ui.add_space(indent);
        
        // Expand/collapse button for nodes with children
        if !node.children.is_empty() {
            // Use ASCII-style indicators that work with all fonts
            let (symbol, new_expanded) = if node.expanded {
                ("-", false)
            } else {
                ("+", true)
            };
            
            // Small button for expand/collapse
            let btn = ui.add(
                egui::Button::new(symbol)
                    .small()
                    .frame(false)
                    .fill(egui::Color32::TRANSPARENT)
            );
            
            if btn.clicked() {
                node.expanded = new_expanded;
            }
        } else {
            // Leaf node - add space for alignment with [+] width
            ui.add_space(20.0);
        }
        
        // Tri-state checkbox - use ASCII-style
        let checkbox_text = match &node.state {
            SelectionState::None => "[ ]",
            SelectionState::All => "[x]",
            SelectionState::Partial(_) => "[~]",
        };
        
        // Use a button for the checkbox
        let checkbox_response = ui.add(
            egui::Button::new(checkbox_text)
                .small()
                .frame(false)
                .fill(egui::Color32::TRANSPARENT)
        );
        
        if checkbox_response.clicked() {
            node.toggle();
        }
        
        // Label
        ui.label(&node.label);
        
        // Show count for parent nodes
        if !node.children.is_empty() {
            let selected = node.count_selected();
            let total = count_all_leaves(node);
            if selected > 0 {
                ui.label(
                    egui::RichText::new(format!("({}/{})", selected, total))
                        .small()
                        .color(egui::Color32::GRAY)
                );
            }
        }
    });
    
    // Draw children if expanded
    if node.expanded {
        for child in &mut node.children {
            draw_tree_node(ui, child, depth + 1);
        }
        // After drawing children, update this node's state based on children
        // This ensures parent states stay synchronized with child selections
        node.update_from_children();
    }
}

/// Count all leaf nodes under a node.
fn count_all_leaves(node: &SelectionNode) -> usize {
    if node.children.is_empty() {
        1
    } else {
        node.children.iter().map(count_all_leaves).sum()
    }
}

/// Draw a tree with a header that can collapse the entire tree.
pub fn draw_tree_with_header(
    ui: &mut egui::Ui, 
    title: &str, 
    node: &mut SelectionNode,
    default_open: bool,
) {
    // Use a collapsing header for the whole tree
    egui::CollapsingHeader::new(title)
        .default_open(default_open)
        .show(ui, |ui| {
            draw_tree(ui, node);
        });
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_count_all_leaves() {
        let mut root = SelectionNode::new("root", "Root");
        root.children.push(SelectionNode::new("c1", "Child 1"));
        root.children.push(SelectionNode::new("c2", "Child 2"));
        
        assert_eq!(count_all_leaves(&root), 2);
        
        let mut nested = SelectionNode::new("n", "Nested");
        nested.children.push(SelectionNode::new("l1", "Leaf 1"));
        nested.children.push(SelectionNode::new("l2", "Leaf 2"));
        root.children.push(nested);
        
        assert_eq!(count_all_leaves(&root), 4);
    }
}
