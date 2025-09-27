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

export const routes: RouteObject[] = [
    {
        path: "/",
        element: <Layout/>,
        children: [
            {index: true, element: <Home/>},
            {path: "experiments", element: <Experiments/>},
            {path: "algorithms", element: <Algorithms/>},
            {path: "algorithms/:id", element: <Algorithm/>},
            {path: "series", element: <SeriesList/>},
            {path: "series/:id", element: <SeriesPage/>},
            {path: "docs", element: <Docs/>},
            {path: "*", element: <NotFound/>},
        ],
    },
];
