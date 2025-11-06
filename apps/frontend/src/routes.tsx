import type { RouteObject } from "react-router-dom";
import Layout from "./components/Layout";
import Home from "./pages/Home";
import Experiments from "./pages/Experiments";
import { lazy } from "react";

const Algorithms = lazy(() => import("./pages/Algorithms"));
const Algorithm = lazy(() => import("./pages/Algorithm"));
const Docs = lazy(() => import("./pages/Docs"));
const SeriesList = lazy(() => import("./pages/Series"));
const SeriesPage = lazy(() => import("./pages/Series/[id]"));
const NotFound = lazy(() => import("./pages/NotFound"));
const Experiments_g1 = lazy(() => import("./pages/Experiments_g1"));
const Experiments_g2 = lazy(() => import("./pages/Experiments_g2"));
const Experiments_g3 = lazy(() => import("./pages/Experiments_g3"));
const Experiments_g4 = lazy(() => import("./pages/Experiments_g4"));




export const routes: RouteObject[] = [
    {
        path: "/",
        element: <Layout/>,
        children: [
            {index: true, element: <Home/>},
            {path: "experiments", element: <Experiments/>},
            {path: "experiments_g1", element: <Experiments_g1/>},
            {path: "experiments_g2", element: <Experiments_g2/>},
            {path: "experiments_g3", element: <Experiments_g3/>},
            {path: "experiments_g4", element: <Experiments_g4/>},
            {path: "algorithms", element: <Algorithms/>},
            {path: "algorithms/:id", element: <Algorithm/>},
            {path: "series", element: <SeriesList/>},
            {path: "series/:id", element: <SeriesPage/>},
            {path: "docs", element: <Docs/>},
            {path: "*", element: <NotFound/>},
        ],
    },
];
