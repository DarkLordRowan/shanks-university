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
const Experiments_v2 = lazy(() => import("./pages/Experiments_v2"));
const Experiments_v3 = lazy(() => import("./pages/Experiments_v3"));
const Experiments_v4 = lazy(() => import("./pages/Experiments_v4"));


export const routes: RouteObject[] = [
    {
        path: "/",
        element: <Layout/>,
        children: [
            {index: true, element: <Home/>},
            {path: "experiments", element: <Experiments/>},
            {path: "experiments_v2", element: <Experiments_v2/>},
            {path: "experiments_v3", element: <Experiments_v3/>},
            {path: "experiments_v4", element: <Experiments_v4/>},
            {path: "algorithms", element: <Algorithms/>},
            {path: "algorithms/:id", element: <Algorithm/>},
            {path: "series", element: <SeriesList/>},
            {path: "series/:id", element: <SeriesPage/>},
            {path: "docs", element: <Docs/>},
            {path: "*", element: <NotFound/>},
        ],
    },
];
