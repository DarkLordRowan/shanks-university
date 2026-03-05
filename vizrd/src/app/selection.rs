use egui::WidgetText;

use crate::experiment::{
    Accel, AccelDef, AccelInstance, ExperimentConfig, Filter, FilterDef, FilterInstance, Noise,
    NoiseDef, NoiseInstance, Series, SeriesDef, SeriesInstance,
};
use std::collections::BTreeMap;

// ─── Selection state ────────────────────────────────────────────────────────

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SelectionState {
    Zero, // monoid identity
    None,
    Some,
    All,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub(crate) struct DrawResult {
    changed: bool,
    state: SelectionState,
}

impl Default for DrawResult {
    fn default() -> Self {
        Self {
            changed: false,
            state: SelectionState::Zero,
        }
    }
}

impl std::ops::BitOr for SelectionState {
    type Output = Self;
    fn bitor(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Self::Zero, a) => a,
            (a, Self::Zero) => a,
            (Self::None, Self::None) => Self::None,
            (Self::All, Self::All) => Self::All,
            _ => Self::Some,
        }
    }
}

impl std::ops::BitOr for DrawResult {
    type Output = Self;
    fn bitor(self, rhs: Self) -> Self::Output {
        DrawResult {
            changed: self.changed | rhs.changed,
            state: self.state | rhs.state,
        }
    }
}
impl std::ops::BitOrAssign for DrawResult {
    fn bitor_assign(&mut self, rhs: Self) {
        *self = *self | rhs;
    }
}

// ─── Block ──────────────────────────────────────────────────────────────────

#[derive(Debug, Clone)]
pub struct Block<T> {
    pub open: bool,
    pub state: SelectionState,
    pub val: T,
}

impl<T> Block<T> {
    pub fn new(val: T) -> Self {
        Self::new_with_state(val, SelectionState::None)
    }
    pub fn new_with_state(val: T, state: SelectionState) -> Self {
        Self {
            open: false,
            state,
            val,
        }
    }
}

// ─── Button ─────────────────────────────────────────────────────────────────

/// A leaf selectable value. `(checked, value, display_label)`.
pub(crate) struct Button<T>(bool, T, String);

fn draw_button(ui: &mut egui::Ui, b: &mut bool, label: impl Into<WidgetText>) -> DrawResult {
    let mut clicked = false;
    ui.horizontal(|ui| {
        let icon = if *b { "☑" } else { "☐" };
        if ui
            .add(egui::Button::new(icon).small().frame(false))
            .clicked()
        {
            *b = !*b;
            clicked = true;
        }
        ui.label(label);
    });
    DrawResult {
        changed: clicked,
        state: if *b {
            SelectionState::All
        } else {
            SelectionState::None
        },
    }
}

// ─── Selectable trait ───────────────────────────────────────────────────────

pub(crate) trait Selectable {
    fn force(&mut self, val: bool);
    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult;
}

impl<T> Selectable for Button<T> {
    fn force(&mut self, val: bool) {
        self.0 = val;
    }
    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        draw_button(ui, &mut self.0, &self.2)
    }
}

impl<T: Selectable> Selectable for Vec<T> {
    fn force(&mut self, val: bool) {
        for i in self {
            i.force(val);
        }
    }
    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        let mut res = DrawResult::default();
        for i in self {
            res |= i.draw(ui);
        }
        res
    }
}

impl<T: Selectable> Block<T> {
    fn force(&mut self, val: bool) {
        self.state = if val {
            SelectionState::All
        } else {
            SelectionState::None
        };
        self.val.force(val);
    }
    fn draw<'a>(&'a mut self, label: impl Into<String>, ui: &mut egui::Ui) -> DrawResult {
        let label: String = label.into();
        let mut changed = false;

        // Header row: [▶/▼] [☐/☑/☒] label — all inline.
        ui.horizontal(|ui| {
            let icon = if self.open { "v" } else { ">" };
            if ui
                .add(egui::Button::new(icon).small().frame(false))
                .clicked()
            {
                self.open = !self.open;
                changed = true;
            }
            let icon = match self.state {
                SelectionState::Zero => "☐",
                SelectionState::None => "☒",
                SelectionState::Some => "~",
                SelectionState::All => "☑",
            };
            if ui
                .add(egui::Button::new(icon).small().frame(false))
                .clicked()
            {
                changed = true;
                self.force(self.state == SelectionState::None);
            }
            ui.label(&label);
        });

        // Children, indented under the header.
        if self.open {
            let indent_id = egui::Id::new(&label).with(ui.next_auto_id());
            ui.indent(indent_id, |ui| {
                let sub = self.val.draw(ui);
                if sub.changed {
                    self.state = sub.state;
                    changed = true;
                }
            });
        }

        DrawResult {
            changed,
            state: self.state,
        }
    }
}

// ─── Arg select types ───────────────────────────────────────────────────────

type ArgSelect = Block<Vec<Button<serde_json::Value>>>;
type ArgStrSelect = Block<Vec<Button<String>>>;
type ArgISelect = Block<Vec<Button<i64>>>;

// ─── Helpers ─────────────────────────────────────────────────────────────────

/// Cartesian product of per-key value lists → `Vec<BTreeMap<key, val>>`.
fn cartesian_combos<V: Clone>(sets: &[(&str, Vec<V>)]) -> Vec<BTreeMap<String, V>> {
    if sets.is_empty() {
        return vec![BTreeMap::new()];
    }
    let mut result: Vec<BTreeMap<String, V>> = vec![BTreeMap::new()];
    for (key, values) in sets {
        let mut next = Vec::new();
        for existing in &result {
            for val in values {
                let mut map = existing.clone();
                map.insert(key.to_string(), val.clone());
                next.push(map);
            }
        }
        result = next;
    }
    result
}

// ─── SeriesSelect ───────────────────────────────────────────────────────────

pub type SeriesSelect = Series<ArgSelect>;

impl Selectable for SeriesSelect {
    fn force(&mut self, val: bool) {
        self.x.force(val);
        for arg in &mut self.args {
            arg.1.force(val);
        }
    }

    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        let mut result = self.x.draw("x", ui);
        for (k, v) in &mut self.args {
            result |= v.draw(k.as_str(), ui);
        }
        result
    }
}

impl Block<SeriesSelect> {
    /// Expand the checked selections into `SeriesInstance`s.
    /// x and each arg dimension are treated as independent cartesian axes.
    pub fn selected_instances(&self) -> Vec<SeriesInstance> {
        if self.state == SelectionState::None {
            return vec![];
        }
        let s = &self.val;

        let x_vals: Vec<serde_json::Value> =
            s.x.val
                .iter()
                .filter(|b| b.0)
                .map(|b| b.1.clone())
                .collect();

        let arg_sets: Vec<(&str, Vec<serde_json::Value>)> = s
            .args
            .iter()
            .filter_map(|(k, ab)| {
                let vals: Vec<_> = ab.val.iter().filter(|b| b.0).map(|b| b.1.clone()).collect();
                if vals.is_empty() {
                    None
                } else {
                    Some((k.as_str(), vals))
                }
            })
            .collect();

        let arg_combos = cartesian_combos(&arg_sets);

        // Cartesian product: each x × each arg combo.
        let mut out = Vec::new();
        for x in &x_vals {
            for args in &arg_combos {
                out.push(SeriesInstance {
                    name: s.name.clone(),
                    x: x.clone(),
                    args: args.clone(),
                });
            }
        }
        // Edge case: no x values selected but some args are — one instance per arg combo (x=null).
        if x_vals.is_empty() && !arg_combos.is_empty() {
            for args in arg_combos {
                out.push(SeriesInstance {
                    name: s.name.clone(),
                    x: serde_json::Value::Null,
                    args,
                });
            }
        }
        out
    }
}

/// Convert a `SeriesDef` into a `SeriesSelect` (all values start unchecked).
fn series_def_to_select(def: &SeriesDef) -> SeriesSelect {
    let x = Block::new(
        def.x
            .iter()
            .map(|v| Button(false, v.clone(), v.to_string()))
            .collect(),
    );
    let args = def
        .args
        .iter()
        .map(|(k, arg)| {
            let block = Block::new(
                arg.iter()
                    .map(|v| Button(false, v.clone(), v.to_string()))
                    .collect(),
            );
            (k.clone(), block)
        })
        .collect();
    SeriesSelect {
        name: def.name.clone(),
        x,
        args,
    }
}

// ─── NoiseSelect ────────────────────────────────────────────────────────────

pub type NoiseSelect = Noise<ArgStrSelect, ArgSelect, ArgISelect>;

impl Selectable for NoiseSelect {
    fn force(&mut self, val: bool) {
        self.method.force(val);
        for arg in &mut self.args {
            arg.1.force(val);
        }
        self.seed.force(val);
    }

    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        let mut result = self.method.draw("Methods", ui);
        for (k, v) in &mut self.args {
            result |= v.draw(k.as_str(), ui);
        }
        result |= self.seed.draw("seed", ui);
        result
    }
}

impl Block<NoiseSelect> {
    /// Cartesian product of (method × args × seed) → `NoiseInstance`s.
    pub fn selected_instances(&self) -> Vec<NoiseInstance> {
        if self.state == SelectionState::None {
            return vec![];
        }

        let n = &self.val;

        let methods: Vec<String> = n
            .method
            .val
            .iter()
            .filter(|b| b.0)
            .map(|b| b.1.clone())
            .collect();
        let seeds: Vec<i64> = n.seed.val.iter().filter(|b| b.0).map(|b| b.1).collect();

        let arg_sets: Vec<(&str, Vec<serde_json::Value>)> = n
            .args
            .iter()
            .filter_map(|(k, ab)| {
                let vals: Vec<_> = ab.val.iter().filter(|b| b.0).map(|b| b.1.clone()).collect();
                if vals.is_empty() {
                    None
                } else {
                    Some((k.as_str(), vals))
                }
            })
            .collect();
        let arg_combos = cartesian_combos(&arg_sets);

        let mut out = Vec::new();
        for method in &methods {
            for seed in &seeds {
                for args in &arg_combos {
                    out.push(NoiseInstance {
                        noise_type: n.noise_type.clone(),
                        method: method.clone(),
                        args: args.clone(),
                        seed: *seed,
                    });
                }
                if arg_combos.is_empty() {
                    out.push(NoiseInstance {
                        noise_type: n.noise_type.clone(),
                        method: method.clone(),
                        args: BTreeMap::new(),
                        seed: *seed,
                    });
                }
            }
        }
        out
    }
}

/// Convert a `NoiseDef` into a `NoiseSelect`.
fn noise_def_to_select(def: &NoiseDef) -> NoiseSelect {
    let method = Block::new(
        def.method
            .iter()
            .map(|s| Button(false, s.to_string(), s.to_string()))
            .collect(),
    );
    let args = def
        .args
        .iter()
        .map(|(k, arg)| {
            let block = Block::new(
                arg.iter()
                    .map(|v| Button(false, v.clone(), v.to_string()))
                    .collect(),
            );
            (k.clone(), block)
        })
        .collect();
    let seed_vals: Vec<i64> = def
        .seed
        .as_ref()
        .map(|s| s.iter().collect())
        .unwrap_or_else(|| vec![0]);
    let seed = Block::new(
        seed_vals
            .into_iter()
            .map(|s| Button(false, s, s.to_string()))
            .collect(),
    );
    NoiseSelect {
        noise_type: def.noise_type.clone(),
        method,
        args,
        seed,
    }
}

// ─── FilterSelect ───────────────────────────────────────────────────────────

pub type FilterSelect = Filter<ArgSelect>;

impl Selectable for FilterSelect {
    fn force(&mut self, val: bool) {
        for arg in &mut self.args {
            arg.1.force(val);
        }
    }

    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        let mut result = DrawResult::default();
        for (k, v) in &mut self.args {
            result |= v.draw(k.as_str(), ui);
        }
        result
    }
}

impl Block<FilterSelect> {
    /// Cartesian product of args → `FilterInstance`s.
    pub fn selected_instances(&self) -> Vec<FilterInstance> {
        if self.state == SelectionState::None {
            return vec![];
        }
        let f = &self.val;
        let arg_sets: Vec<(&str, Vec<serde_json::Value>)> = f
            .args
            .iter()
            .filter_map(|(k, ab)| {
                let vals: Vec<_> = ab.val.iter().filter(|b| b.0).map(|b| b.1.clone()).collect();
                if vals.is_empty() {
                    None
                } else {
                    Some((k.as_str(), vals))
                }
            })
            .collect();
        cartesian_combos(&arg_sets)
            .into_iter()
            .map(|args| FilterInstance {
                filter_type: f.filter_type.clone(),
                args,
            })
            .collect()
    }
}

/// Convert a `FilterDef` into a `FilterSelect`.
fn filter_def_to_select(def: &FilterDef) -> FilterSelect {
    let args = def
        .args
        .iter()
        .map(|(k, arg)| {
            let block = Block::new(
                arg.iter()
                    .map(|v| Button(false, v.clone(), v.to_string()))
                    .collect(),
            );
            (k.clone(), block)
        })
        .collect();
    FilterSelect {
        filter_type: def.filter_type.clone(),
        args,
    }
}

// ─── AccelSelect ────────────────────────────────────────────────────────────

pub type AccelSelect = Accel<ArgISelect, ArgSelect>;

impl Selectable for AccelSelect {
    fn force(&mut self, val: bool) {
        self.m.force(val);
        for (_, v) in &mut self.args {
            v.force(val);
        }
    }

    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        let mut result = self.m.draw("m", ui);
        for (k, v) in &mut self.args {
            result |= v.draw(k, ui);
        }
        result
    }
}

impl Block<AccelSelect> {
    /// Cartesian product of (m × args) → `AccelInstance`s.
    pub fn selected_instances(&self) -> Vec<AccelInstance> {
        if self.state == SelectionState::None {
            return vec![];
        }
        let a = &self.val;
        let ms: Vec<i64> = a.m.val.iter().filter(|b| b.0).map(|b| b.1).collect();
        let arg_sets: Vec<(&str, Vec<serde_json::Value>)> = a
            .args
            .iter()
            .filter_map(|(k, ab)| {
                let vals: Vec<_> = ab.val.iter().filter(|b| b.0).map(|b| b.1.clone()).collect();
                if vals.is_empty() {
                    None
                } else {
                    Some((k.as_str(), vals))
                }
            })
            .collect();
        let arg_combos = cartesian_combos(&arg_sets);

        let mut out = Vec::new();
        for m in &ms {
            for args in &arg_combos {
                out.push(AccelInstance {
                    name: a.name.clone(),
                    m: *m,
                    args: args.clone(),
                    events: a.events.clone(),
                });
            }
            if arg_combos.is_empty() {
                out.push(AccelInstance {
                    name: a.name.clone(),
                    m: *m,
                    args: BTreeMap::new(),
                    events: a.events.clone(),
                });
            }
        }
        out
    }
}

/// Convert an `AccelDef` into an `AccelSelect`.
fn accel_def_to_select(def: &AccelDef) -> AccelSelect {
    let m = Block::new(
        def.m
            .iter()
            .map(|v| Button(false, v, v.to_string()))
            .collect(),
    );
    let args = def
        .args
        .iter()
        .map(|(k, arg)| {
            let block = Block::new(
                arg.iter()
                    .map(|v| Button(false, v.clone(), v.to_string()))
                    .collect(),
            );
            (k.clone(), block)
        })
        .collect();
    AccelSelect {
        name: def.name.clone(),
        m,
        args,
        events: def.events.clone(),
    }
}

struct NoiseVecSelect(Vec<Block<NoiseSelect>>, bool);
impl Selectable for NoiseVecSelect {
    fn force(&mut self, val: bool) {
        self.1 = val;
        self.0.force(val);
    }

    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        let mut res = draw_button(ui, &mut self.1, "No noise");
        res | self.0.draw(ui)
    }
}

// ─── Blockable (UI helper) ───────────────────────────────────────────────────

impl<T: Selectable> Selectable for Option<T> {
    fn force(&mut self, val: bool) {
        self.as_mut().map(|x| x.force(val));
    }

    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        self.as_mut().map_or(DrawResult::default(), |x| x.draw(ui))
    }
}

trait Blockable: Selectable {
    fn name(&self) -> &str;
}
impl Blockable for SeriesSelect {
    fn name(&self) -> &str {
        &self.name
    }
}
impl Blockable for FilterSelect {
    fn name(&self) -> &str {
        &self.filter_type
    }
}
impl Blockable for NoiseSelect {
    fn name(&self) -> &str {
        &self.noise_type
    }
}
impl Blockable for AccelSelect {
    fn name(&self) -> &str {
        &self.name
    }
}
impl<T: Blockable> Selectable for Block<T> {
    fn force(&mut self, val: bool) {
        self.force(val);
    }
    fn draw(&mut self, ui: &mut egui::Ui) -> DrawResult {
        self.draw(self.val.name().to_owned(), ui)
    }
}

// ─── AppSelection (coordinator-facing) ──────────────────────────────────────

/// The set of instances the coordinator should compute.
/// `noises` contains `None` to represent "no noise" (plain series).
#[derive(Debug, Clone, Default)]
pub struct AppSelection {
    pub precisions: Vec<String>,
    pub series: Vec<SeriesInstance>,
    pub noises: Vec<Option<NoiseInstance>>,
    pub accels: Vec<AccelInstance>,
    pub filters: Vec<FilterInstance>,
}

// ─── AppSelect (UI owning tree) ──────────────────────────────────────────────

pub struct AppSelect {
    series: Block<Vec<Block<SeriesSelect>>>,
    noise: Block<NoiseVecSelect>,
    filter: Block<Vec<Block<FilterSelect>>>,
    accel: Block<Vec<Block<AccelSelect>>>,
    precision: Block<Vec<Button<String>>>,
}

impl AppSelect {
    /// Build from an `ExperimentConfig`. All values start deselected.
    pub fn from_config(exp: &ExperimentConfig) -> Self {
        let series = Block::new(
            exp.series
                .iter()
                .map(|def| Block::new(series_def_to_select(def)))
                .collect(),
        );
        let noises: Vec<Block<NoiseSelect>> = exp
            .noises
            .iter()
            .map(|def| Block::new(noise_def_to_select(def)))
            .collect();
        let no_noises = noises.is_empty();
        let noise = Block::new_with_state(
            NoiseVecSelect(noises, true),
            if no_noises {
                SelectionState::All
            } else {
                SelectionState::Some
            },
        );
        let filter = Block::new(
            exp.filters
                .iter()
                .map(|def| Block::new(filter_def_to_select(def)))
                .collect(),
        );
        let accel = Block::new(
            exp.accels
                .iter()
                .map(|def| Block::new(accel_def_to_select(def)))
                .collect(),
        );
        let precisions = exp.precisions.clone().unwrap_or_default();
        let precision = Block::new(
            precisions
                .into_iter()
                .map(|p| Button(false, p.clone(), p))
                .collect(),
        );
        Self {
            series,
            noise,
            filter,
            accel,
            precision,
        }
    }

    /// Draw the UI panel and return whether anything changed.
    pub fn draw(&mut self, ui: &mut egui::Ui) -> bool {
        let mut changed = false;
        if self.series.draw("Series", ui).changed {
            changed = true;
        }
        if self.noise.draw("Noise", ui).changed {
            changed = true;
        }
        if self.filter.draw("Filter", ui).changed {
            changed = true;
        }
        if self.accel.draw("Accel", ui).changed {
            changed = true;
        }
        if self.precision.draw("Precision", ui).changed {
            changed = true;
        }
        changed
    }

    /// Extract the current checked state as typed instances for the coordinator.
    pub fn extract(&self) -> AppSelection {
        let series: Vec<SeriesInstance> = self
            .series
            .val
            .iter()
            .flat_map(|b| b.selected_instances())
            .collect();

        let mut noises: Vec<Option<NoiseInstance>> = self
            .noise
            .val
            .0
            .iter()
            .flat_map(|b| b.selected_instances().into_iter())
            .map(Some)
            .collect();
        if self.noise.val.1 {
            noises.push(None);
        }

        let filters: Vec<FilterInstance> = self
            .filter
            .val
            .iter()
            .flat_map(|b| b.selected_instances())
            .collect();

        let accels: Vec<AccelInstance> = self
            .accel
            .val
            .iter()
            .flat_map(|b| b.selected_instances())
            .collect();

        let precisions: Vec<String> = self
            .precision
            .val
            .iter()
            .filter(|b| b.0)
            .map(|b| b.1.clone())
            .collect();

        AppSelection {
            precisions,
            series,
            noises,
            accels,
            filters,
        }
    }
}
