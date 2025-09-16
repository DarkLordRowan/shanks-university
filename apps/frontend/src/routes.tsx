import type { RouteObject } from "react-router-dom";
import Layout from "./components/Layout";
import Home from "./pages/Home";
import Experiments from "./pages/Experiments";
import Algorithms from "./pages/Algorithms";
import Docs from "./pages/Docs";
import NotFound from "./pages/NotFound";
import Algorithm from "./pages/Algorithm";
import SeriesList from "./pages/Series";
import SeriesPage from "./pages/Series/[id]";

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
