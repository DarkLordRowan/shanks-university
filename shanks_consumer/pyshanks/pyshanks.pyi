"""
pybind11: polymorphic series (double + float_precision), helper-organized, no-arg constructors, backward-compatible
"""

from __future__ import annotations

import typing

__all__: list[str] = [
    "AbsSinXMinus2DivPiSeriesArb",
    "AbsSinXMinus2DivPiSeriesCArb",
    "AbsSinXMinus2DivPiSeriesCF32",
    "AbsSinXMinus2DivPiSeriesCF64",
    "AbsSinXMinus2DivPiSeriesCFLong",
    "AbsSinXMinus2DivPiSeriesF32",
    "AbsSinXMinus2DivPiSeriesF64",
    "AbsSinXMinus2DivPiSeriesFLong",
    "Arb",
    "ArcsinX2SeriesArb",
    "ArcsinX2SeriesCArb",
    "ArcsinX2SeriesCF32",
    "ArcsinX2SeriesCF64",
    "ArcsinX2SeriesCFLong",
    "ArcsinX2SeriesF32",
    "ArcsinX2SeriesF64",
    "ArcsinX2SeriesFLong",
    "ArcsinXMinusXSeriesArb",
    "ArcsinXMinusXSeriesCArb",
    "ArcsinXMinusXSeriesCF32",
    "ArcsinXMinusXSeriesCF64",
    "ArcsinXMinusXSeriesCFLong",
    "ArcsinXMinusXSeriesF32",
    "ArcsinXMinusXSeriesF64",
    "ArcsinXMinusXSeriesFLong",
    "ArcsinXSeriesArb",
    "ArcsinXSeriesCArb",
    "ArcsinXSeriesCF32",
    "ArcsinXSeriesCF64",
    "ArcsinXSeriesCFLong",
    "ArcsinXSeriesF32",
    "ArcsinXSeriesF64",
    "ArcsinXSeriesFLong",
    "ArcsinhXSeriesArb",
    "ArcsinhXSeriesCArb",
    "ArcsinhXSeriesCF32",
    "ArcsinhXSeriesCF64",
    "ArcsinhXSeriesCFLong",
    "ArcsinhXSeriesF32",
    "ArcsinhXSeriesF64",
    "ArcsinhXSeriesFLong",
    "ArctanhX2SeriesArb",
    "ArctanhX2SeriesCArb",
    "ArctanhX2SeriesCF32",
    "ArctanhX2SeriesCF64",
    "ArctanhX2SeriesCFLong",
    "ArctanhX2SeriesF32",
    "ArctanhX2SeriesF64",
    "ArctanhX2SeriesFLong",
    "ArctanhXSeriesArb",
    "ArctanhXSeriesCArb",
    "ArctanhXSeriesCF32",
    "ArctanhXSeriesCF64",
    "ArctanhXSeriesCFLong",
    "ArctanhXSeriesF32",
    "ArctanhXSeriesF64",
    "ArctanhXSeriesFLong",
    "ArctgX2SeriesArb",
    "ArctgX2SeriesCArb",
    "ArctgX2SeriesCF32",
    "ArctgX2SeriesCF64",
    "ArctgX2SeriesCFLong",
    "ArctgX2SeriesF32",
    "ArctgX2SeriesF64",
    "ArctgX2SeriesFLong",
    "ArctgX3SeriesArb",
    "ArctgX3SeriesCArb",
    "ArctgX3SeriesCF32",
    "ArctgX3SeriesCF64",
    "ArctgX3SeriesCFLong",
    "ArctgX3SeriesF32",
    "ArctgX3SeriesF64",
    "ArctgX3SeriesFLong",
    "ArctgXSeriesArb",
    "ArctgXSeriesCArb",
    "ArctgXSeriesCF32",
    "ArctgXSeriesCF64",
    "ArctgXSeriesCFLong",
    "ArctgXSeriesF32",
    "ArctgXSeriesF64",
    "ArctgXSeriesFLong",
    "BinSeriesArb",
    "BinSeriesCArb",
    "BinSeriesCF32",
    "BinSeriesCF64",
    "BinSeriesCFLong",
    "BinSeriesF32",
    "BinSeriesF64",
    "BinSeriesFLong",
    "BrezinskiThetaAlgorithmArb",
    "BrezinskiThetaAlgorithmCArb",
    "BrezinskiThetaAlgorithmCF32",
    "BrezinskiThetaAlgorithmCF64",
    "BrezinskiThetaAlgorithmCFLong",
    "BrezinskiThetaAlgorithmF32",
    "BrezinskiThetaAlgorithmF64",
    "BrezinskiThetaAlgorithmFLong",
    "CArb",
    "CF32",
    "CF64",
    "CFLong",
    "ChangWynnAlgorithmArb",
    "ChangWynnAlgorithmCArb",
    "ChangWynnAlgorithmCF32",
    "ChangWynnAlgorithmCF64",
    "ChangWynnAlgorithmCFLong",
    "ChangWynnAlgorithmF32",
    "ChangWynnAlgorithmF64",
    "ChangWynnAlgorithmFLong",
    "CiXSeriesArb",
    "CiXSeriesCArb",
    "CiXSeriesCF32",
    "CiXSeriesCF64",
    "CiXSeriesCFLong",
    "CiXSeriesF32",
    "CiXSeriesF64",
    "CiXSeriesFLong",
    "Cos3xMinus1DivXsquareSeriesArb",
    "Cos3xMinus1DivXsquareSeriesCArb",
    "Cos3xMinus1DivXsquareSeriesCF32",
    "Cos3xMinus1DivXsquareSeriesCF64",
    "Cos3xMinus1DivXsquareSeriesCFLong",
    "Cos3xMinus1DivXsquareSeriesF32",
    "Cos3xMinus1DivXsquareSeriesF64",
    "Cos3xMinus1DivXsquareSeriesFLong",
    "CosSeriesArb",
    "CosSeriesCArb",
    "CosSeriesCF32",
    "CosSeriesCF64",
    "CosSeriesCFLong",
    "CosSeriesF32",
    "CosSeriesF64",
    "CosSeriesFLong",
    "CosSqrtXSeriesArb",
    "CosSqrtXSeriesCArb",
    "CosSqrtXSeriesCF32",
    "CosSqrtXSeriesCF64",
    "CosSqrtXSeriesCFLong",
    "CosSqrtXSeriesF32",
    "CosSqrtXSeriesF64",
    "CosSqrtXSeriesFLong",
    "CosX2SeriesArb",
    "CosX2SeriesCArb",
    "CosX2SeriesCF32",
    "CosX2SeriesCF64",
    "CosX2SeriesCFLong",
    "CosX2SeriesF32",
    "CosX2SeriesF64",
    "CosX2SeriesFLong",
    "CoshSeriesArb",
    "CoshSeriesCArb",
    "CoshSeriesCF32",
    "CoshSeriesCF64",
    "CoshSeriesCFLong",
    "CoshSeriesF32",
    "CoshSeriesF64",
    "CoshSeriesFLong",
    "DrummondDAlgorithmArb",
    "DrummondDAlgorithmCArb",
    "DrummondDAlgorithmCF32",
    "DrummondDAlgorithmCF64",
    "DrummondDAlgorithmCFLong",
    "DrummondDAlgorithmF32",
    "DrummondDAlgorithmF64",
    "DrummondDAlgorithmFLong",
    "EXSeriesArb",
    "EXSeriesCArb",
    "EXSeriesCF32",
    "EXSeriesCF64",
    "EXSeriesCFLong",
    "EXSeriesF32",
    "EXSeriesF64",
    "EXSeriesFLong",
    "EighthPiMOneThirdSeriesArb",
    "EighthPiMOneThirdSeriesCArb",
    "EighthPiMOneThirdSeriesCF32",
    "EighthPiMOneThirdSeriesCF64",
    "EighthPiMOneThirdSeriesCFLong",
    "EighthPiMOneThirdSeriesF32",
    "EighthPiMOneThirdSeriesF64",
    "EighthPiMOneThirdSeriesFLong",
    "ErfSeriesArb",
    "ErfSeriesCArb",
    "ErfSeriesCF32",
    "ErfSeriesCF64",
    "ErfSeriesCFLong",
    "ErfSeriesF32",
    "ErfSeriesF64",
    "ErfSeriesFLong",
    "ExpMCosXSinsinXSeriesArb",
    "ExpMCosXSinsinXSeriesCArb",
    "ExpMCosXSinsinXSeriesCF32",
    "ExpMCosXSinsinXSeriesCF64",
    "ExpMCosXSinsinXSeriesCFLong",
    "ExpMCosXSinsinXSeriesF32",
    "ExpMCosXSinsinXSeriesF64",
    "ExpMCosXSinsinXSeriesFLong",
    "ExpSeriesArb",
    "ExpSeriesCArb",
    "ExpSeriesCF32",
    "ExpSeriesCF64",
    "ExpSeriesCFLong",
    "ExpSeriesF32",
    "ExpSeriesF64",
    "ExpSeriesFLong",
    "ExpSquaredErfSeriesArb",
    "ExpSquaredErfSeriesCArb",
    "ExpSquaredErfSeriesCF32",
    "ExpSquaredErfSeriesCF64",
    "ExpSquaredErfSeriesCFLong",
    "ExpSquaredErfSeriesF32",
    "ExpSquaredErfSeriesF64",
    "ExpSquaredErfSeriesFLong",
    "FivePiTwelveSeriesArb",
    "FivePiTwelveSeriesCArb",
    "FivePiTwelveSeriesCF32",
    "FivePiTwelveSeriesCF64",
    "FivePiTwelveSeriesCFLong",
    "FivePiTwelveSeriesF32",
    "FivePiTwelveSeriesF64",
    "FivePiTwelveSeriesFLong",
    "FordSidi2AlgorithmArb",
    "FordSidi2AlgorithmCArb",
    "FordSidi2AlgorithmCF32",
    "FordSidi2AlgorithmCF64",
    "FordSidi2AlgorithmCFLong",
    "FordSidi2AlgorithmF32",
    "FordSidi2AlgorithmF64",
    "FordSidi2AlgorithmFLong",
    "FordSidi3AlgorithmArb",
    "FordSidi3AlgorithmCArb",
    "FordSidi3AlgorithmCF32",
    "FordSidi3AlgorithmCF64",
    "FordSidi3AlgorithmCFLong",
    "FordSidi3AlgorithmF32",
    "FordSidi3AlgorithmF64",
    "FordSidi3AlgorithmFLong",
    "FourArctanSeriesArb",
    "FourArctanSeriesCArb",
    "FourArctanSeriesCF32",
    "FourArctanSeriesCF64",
    "FourArctanSeriesCFLong",
    "FourArctanSeriesF32",
    "FourArctanSeriesF64",
    "FourArctanSeriesFLong",
    "FourLn2M3SeriesArb",
    "FourLn2M3SeriesCArb",
    "FourLn2M3SeriesCF32",
    "FourLn2M3SeriesCF64",
    "FourLn2M3SeriesCFLong",
    "FourLn2M3SeriesF32",
    "FourLn2M3SeriesF64",
    "FourLn2M3SeriesFLong",
    "HalfAsinTwoXSeriesArb",
    "HalfAsinTwoXSeriesCArb",
    "HalfAsinTwoXSeriesCF32",
    "HalfAsinTwoXSeriesCF64",
    "HalfAsinTwoXSeriesCFLong",
    "HalfAsinTwoXSeriesF32",
    "HalfAsinTwoXSeriesF64",
    "HalfAsinTwoXSeriesFLong",
    "HalfMinusSinxMultiPi4SeriesArb",
    "HalfMinusSinxMultiPi4SeriesCArb",
    "HalfMinusSinxMultiPi4SeriesCF32",
    "HalfMinusSinxMultiPi4SeriesCF64",
    "HalfMinusSinxMultiPi4SeriesCFLong",
    "HalfMinusSinxMultiPi4SeriesF32",
    "HalfMinusSinxMultiPi4SeriesF64",
    "HalfMinusSinxMultiPi4SeriesFLong",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesArb",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesCArb",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesCF32",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesCF64",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesCFLong",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesF32",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesF64",
    "HalfMultiLn1Div2Multi1MinusCosxSeriesFLong",
    "IncompleteGammaFuncSeriesArb",
    "IncompleteGammaFuncSeriesCArb",
    "IncompleteGammaFuncSeriesCF32",
    "IncompleteGammaFuncSeriesCF64",
    "IncompleteGammaFuncSeriesCFLong",
    "IncompleteGammaFuncSeriesF32",
    "IncompleteGammaFuncSeriesF64",
    "IncompleteGammaFuncSeriesFLong",
    "InvSqrt1m4xSeriesArb",
    "InvSqrt1m4xSeriesCArb",
    "InvSqrt1m4xSeriesCF32",
    "InvSqrt1m4xSeriesCF64",
    "InvSqrt1m4xSeriesCFLong",
    "InvSqrt1m4xSeriesF32",
    "InvSqrt1m4xSeriesF64",
    "InvSqrt1m4xSeriesFLong",
    "Inverse1mxSeriesArb",
    "Inverse1mxSeriesCArb",
    "Inverse1mxSeriesCF32",
    "Inverse1mxSeriesCF64",
    "Inverse1mxSeriesCFLong",
    "Inverse1mxSeriesF32",
    "Inverse1mxSeriesF64",
    "Inverse1mxSeriesFLong",
    "KXSeriesArb",
    "KXSeriesCArb",
    "KXSeriesCF32",
    "KXSeriesCF64",
    "KXSeriesCFLong",
    "KXSeriesF32",
    "KXSeriesF64",
    "KXSeriesFLong",
    "LambertWFuncSeriesArb",
    "LambertWFuncSeriesCArb",
    "LambertWFuncSeriesCF32",
    "LambertWFuncSeriesCF64",
    "LambertWFuncSeriesCFLong",
    "LambertWFuncSeriesF32",
    "LambertWFuncSeriesF64",
    "LambertWFuncSeriesFLong",
    "LevinAlgorithmArb",
    "LevinAlgorithmCArb",
    "LevinAlgorithmCF32",
    "LevinAlgorithmCF64",
    "LevinAlgorithmCFLong",
    "LevinAlgorithmF32",
    "LevinAlgorithmF64",
    "LevinAlgorithmFLong",
    "LevinSidiMAlgorithmArb",
    "LevinSidiMAlgorithmCArb",
    "LevinSidiMAlgorithmCF32",
    "LevinSidiMAlgorithmCF64",
    "LevinSidiMAlgorithmCFLong",
    "LevinSidiMAlgorithmF32",
    "LevinSidiMAlgorithmF64",
    "LevinSidiMAlgorithmFLong",
    "LevinSidiSAlgorithmArb",
    "LevinSidiSAlgorithmCArb",
    "LevinSidiSAlgorithmCF32",
    "LevinSidiSAlgorithmCF64",
    "LevinSidiSAlgorithmCFLong",
    "LevinSidiSAlgorithmF32",
    "LevinSidiSAlgorithmF64",
    "LevinSidiSAlgorithmFLong",
    "Ln13MinusLn7Div7SeriesArb",
    "Ln13MinusLn7Div7SeriesCArb",
    "Ln13MinusLn7Div7SeriesCF32",
    "Ln13MinusLn7Div7SeriesCF64",
    "Ln13MinusLn7Div7SeriesCFLong",
    "Ln13MinusLn7Div7SeriesF32",
    "Ln13MinusLn7Div7SeriesF64",
    "Ln13MinusLn7Div7SeriesFLong",
    "Ln1MinusX2SeriesArb",
    "Ln1MinusX2SeriesCArb",
    "Ln1MinusX2SeriesCF32",
    "Ln1MinusX2SeriesCF64",
    "Ln1MinusX2SeriesCFLong",
    "Ln1MinusX2SeriesF32",
    "Ln1MinusX2SeriesF64",
    "Ln1MinusX2SeriesFLong",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesArb",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCArb",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCF32",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCF64",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCFLong",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesF32",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesF64",
    "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesFLong",
    "Ln1PlusX3SeriesArb",
    "Ln1PlusX3SeriesCArb",
    "Ln1PlusX3SeriesCF32",
    "Ln1PlusX3SeriesCF64",
    "Ln1PlusX3SeriesCFLong",
    "Ln1PlusX3SeriesF32",
    "Ln1PlusX3SeriesF64",
    "Ln1PlusX3SeriesFLong",
    "Ln1mxSeriesArb",
    "Ln1mxSeriesCArb",
    "Ln1mxSeriesCF32",
    "Ln1mxSeriesCF64",
    "Ln1mxSeriesCFLong",
    "Ln1mxSeriesF32",
    "Ln1mxSeriesF64",
    "Ln1mxSeriesFLong",
    "Ln1plusXDiv1plusX2SeriesArb",
    "Ln1plusXDiv1plusX2SeriesCArb",
    "Ln1plusXDiv1plusX2SeriesCF32",
    "Ln1plusXDiv1plusX2SeriesCF64",
    "Ln1plusXDiv1plusX2SeriesCFLong",
    "Ln1plusXDiv1plusX2SeriesF32",
    "Ln1plusXDiv1plusX2SeriesF64",
    "Ln1plusXDiv1plusX2SeriesFLong",
    "Ln1px4SeriesArb",
    "Ln1px4SeriesCArb",
    "Ln1px4SeriesCF32",
    "Ln1px4SeriesCF64",
    "Ln1px4SeriesCFLong",
    "Ln1px4SeriesF32",
    "Ln1px4SeriesF64",
    "Ln1px4SeriesFLong",
    "Ln2SeriesArb",
    "Ln2SeriesCArb",
    "Ln2SeriesCF32",
    "Ln2SeriesCF64",
    "Ln2SeriesCFLong",
    "Ln2SeriesF32",
    "Ln2SeriesF64",
    "Ln2SeriesFLong",
    "LnCosxSeriesArb",
    "LnCosxSeriesCArb",
    "LnCosxSeriesCF32",
    "LnCosxSeriesCF64",
    "LnCosxSeriesCFLong",
    "LnCosxSeriesF32",
    "LnCosxSeriesF64",
    "LnCosxSeriesFLong",
    "LnSinxMinusLnXSeriesArb",
    "LnSinxMinusLnXSeriesCArb",
    "LnSinxMinusLnXSeriesCF32",
    "LnSinxMinusLnXSeriesCF64",
    "LnSinxMinusLnXSeriesCFLong",
    "LnSinxMinusLnXSeriesF32",
    "LnSinxMinusLnXSeriesF64",
    "LnSinxMinusLnXSeriesFLong",
    "LnXPlusOneXMinusOneHalfedSeriesArb",
    "LnXPlusOneXMinusOneHalfedSeriesCArb",
    "LnXPlusOneXMinusOneHalfedSeriesCF32",
    "LnXPlusOneXMinusOneHalfedSeriesCF64",
    "LnXPlusOneXMinusOneHalfedSeriesCFLong",
    "LnXPlusOneXMinusOneHalfedSeriesF32",
    "LnXPlusOneXMinusOneHalfedSeriesF64",
    "LnXPlusOneXMinusOneHalfedSeriesFLong",
    "LubkinWAlgorithmArb",
    "LubkinWAlgorithmCArb",
    "LubkinWAlgorithmCF32",
    "LubkinWAlgorithmCF64",
    "LubkinWAlgorithmCFLong",
    "LubkinWAlgorithmF32",
    "LubkinWAlgorithmF64",
    "LubkinWAlgorithmFLong",
    "MFact1mxMp1InverseSeriesArb",
    "MFact1mxMp1InverseSeriesCArb",
    "MFact1mxMp1InverseSeriesCF32",
    "MFact1mxMp1InverseSeriesCF64",
    "MFact1mxMp1InverseSeriesCFLong",
    "MFact1mxMp1InverseSeriesF32",
    "MFact1mxMp1InverseSeriesF64",
    "MFact1mxMp1InverseSeriesFLong",
    "MeanSinhSinSeriesArb",
    "MeanSinhSinSeriesCArb",
    "MeanSinhSinSeriesCF32",
    "MeanSinhSinSeriesCF64",
    "MeanSinhSinSeriesCFLong",
    "MeanSinhSinSeriesF32",
    "MeanSinhSinSeriesF64",
    "MeanSinhSinSeriesFLong",
    "Minus3Div4OrXMinus3Div4SeriesArb",
    "Minus3Div4OrXMinus3Div4SeriesCArb",
    "Minus3Div4OrXMinus3Div4SeriesCF32",
    "Minus3Div4OrXMinus3Div4SeriesCF64",
    "Minus3Div4OrXMinus3Div4SeriesCFLong",
    "Minus3Div4OrXMinus3Div4SeriesF32",
    "Minus3Div4OrXMinus3Div4SeriesF64",
    "Minus3Div4OrXMinus3Div4SeriesFLong",
    "MinusOneNFactNInNSeriesArb",
    "MinusOneNFactNInNSeriesCArb",
    "MinusOneNFactNInNSeriesCF32",
    "MinusOneNFactNInNSeriesCF64",
    "MinusOneNFactNInNSeriesCFLong",
    "MinusOneNFactNInNSeriesF32",
    "MinusOneNFactNInNSeriesF64",
    "MinusOneNFactNInNSeriesFLong",
    "MinusOneNedInNSeriesArb",
    "MinusOneNedInNSeriesCArb",
    "MinusOneNedInNSeriesCF32",
    "MinusOneNedInNSeriesCF64",
    "MinusOneNedInNSeriesCFLong",
    "MinusOneNedInNSeriesF32",
    "MinusOneNedInNSeriesF64",
    "MinusOneNedInNSeriesFLong",
    "MinusOneQuarterSeriesArb",
    "MinusOneQuarterSeriesCArb",
    "MinusOneQuarterSeriesCF32",
    "MinusOneQuarterSeriesCF64",
    "MinusOneQuarterSeriesCFLong",
    "MinusOneQuarterSeriesF32",
    "MinusOneQuarterSeriesF64",
    "MinusOneQuarterSeriesFLong",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesArb",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCArb",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCF32",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCF64",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCFLong",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesF32",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesF64",
    "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesFLong",
    "MinusXMinusPi4OrMinusPi4SeriesArb",
    "MinusXMinusPi4OrMinusPi4SeriesCArb",
    "MinusXMinusPi4OrMinusPi4SeriesCF32",
    "MinusXMinusPi4OrMinusPi4SeriesCF64",
    "MinusXMinusPi4OrMinusPi4SeriesCFLong",
    "MinusXMinusPi4OrMinusPi4SeriesF32",
    "MinusXMinusPi4OrMinusPi4SeriesF64",
    "MinusXMinusPi4OrMinusPi4SeriesFLong",
    "NumeratorType",
    "OneDivSqrt2SinXDivSqrt2SeriesArb",
    "OneDivSqrt2SinXDivSqrt2SeriesCArb",
    "OneDivSqrt2SinXDivSqrt2SeriesCF32",
    "OneDivSqrt2SinXDivSqrt2SeriesCF64",
    "OneDivSqrt2SinXDivSqrt2SeriesCFLong",
    "OneDivSqrt2SinXDivSqrt2SeriesF32",
    "OneDivSqrt2SinXDivSqrt2SeriesF64",
    "OneDivSqrt2SinXDivSqrt2SeriesFLong",
    "OneDivTwoMinusXMultiThreePlusXSeriesArb",
    "OneDivTwoMinusXMultiThreePlusXSeriesCArb",
    "OneDivTwoMinusXMultiThreePlusXSeriesCF32",
    "OneDivTwoMinusXMultiThreePlusXSeriesCF64",
    "OneDivTwoMinusXMultiThreePlusXSeriesCFLong",
    "OneDivTwoMinusXMultiThreePlusXSeriesF32",
    "OneDivTwoMinusXMultiThreePlusXSeriesF64",
    "OneDivTwoMinusXMultiThreePlusXSeriesFLong",
    "OneMinusSqrt1minus4xDiv2xSeriesArb",
    "OneMinusSqrt1minus4xDiv2xSeriesCArb",
    "OneMinusSqrt1minus4xDiv2xSeriesCF32",
    "OneMinusSqrt1minus4xDiv2xSeriesCF64",
    "OneMinusSqrt1minus4xDiv2xSeriesCFLong",
    "OneMinusSqrt1minus4xDiv2xSeriesF32",
    "OneMinusSqrt1minus4xDiv2xSeriesF64",
    "OneMinusSqrt1minus4xDiv2xSeriesFLong",
    "OneSeriesArb",
    "OneSeriesCArb",
    "OneSeriesCF32",
    "OneSeriesCF64",
    "OneSeriesCFLong",
    "OneSeriesF32",
    "OneSeriesF64",
    "OneSeriesFLong",
    "OneThirdPiSquaredMNineSeriesArb",
    "OneThirdPiSquaredMNineSeriesCArb",
    "OneThirdPiSquaredMNineSeriesCF32",
    "OneThirdPiSquaredMNineSeriesCF64",
    "OneThirdPiSquaredMNineSeriesCFLong",
    "OneThirdPiSquaredMNineSeriesF32",
    "OneThirdPiSquaredMNineSeriesF64",
    "OneThirdPiSquaredMNineSeriesFLong",
    "OneTwelfth3x2Pi2SeriesArb",
    "OneTwelfth3x2Pi2SeriesCArb",
    "OneTwelfth3x2Pi2SeriesCF32",
    "OneTwelfth3x2Pi2SeriesCF64",
    "OneTwelfth3x2Pi2SeriesCFLong",
    "OneTwelfth3x2Pi2SeriesF32",
    "OneTwelfth3x2Pi2SeriesF64",
    "OneTwelfth3x2Pi2SeriesFLong",
    "OneTwelfthSeriesArb",
    "OneTwelfthSeriesCArb",
    "OneTwelfthSeriesCF32",
    "OneTwelfthSeriesCF64",
    "OneTwelfthSeriesCFLong",
    "OneTwelfthSeriesF32",
    "OneTwelfthSeriesF64",
    "OneTwelfthSeriesFLong",
    "OneTwelfthX2Pi2SeriesArb",
    "OneTwelfthX2Pi2SeriesCArb",
    "OneTwelfthX2Pi2SeriesCF32",
    "OneTwelfthX2Pi2SeriesCF64",
    "OneTwelfthX2Pi2SeriesCFLong",
    "OneTwelfthX2Pi2SeriesF32",
    "OneTwelfthX2Pi2SeriesF64",
    "OneTwelfthX2Pi2SeriesFLong",
    "Pi3SeriesArb",
    "Pi3SeriesCArb",
    "Pi3SeriesCF32",
    "Pi3SeriesCF64",
    "Pi3SeriesCFLong",
    "Pi3SeriesF32",
    "Pi3SeriesF64",
    "Pi3SeriesFLong",
    "Pi4SeriesArb",
    "Pi4SeriesCArb",
    "Pi4SeriesCF32",
    "Pi4SeriesCF64",
    "Pi4SeriesCFLong",
    "Pi4SeriesF32",
    "Pi4SeriesF64",
    "Pi4SeriesFLong",
    "Pi8CosxSquareMinus1Div3CosxSeriesArb",
    "Pi8CosxSquareMinus1Div3CosxSeriesCArb",
    "Pi8CosxSquareMinus1Div3CosxSeriesCF32",
    "Pi8CosxSquareMinus1Div3CosxSeriesCF64",
    "Pi8CosxSquareMinus1Div3CosxSeriesCFLong",
    "Pi8CosxSquareMinus1Div3CosxSeriesF32",
    "Pi8CosxSquareMinus1Div3CosxSeriesF64",
    "Pi8CosxSquareMinus1Div3CosxSeriesFLong",
    "PiCubed32SeriesArb",
    "PiCubed32SeriesCArb",
    "PiCubed32SeriesCF32",
    "PiCubed32SeriesCF64",
    "PiCubed32SeriesCFLong",
    "PiCubed32SeriesF32",
    "PiCubed32SeriesF64",
    "PiCubed32SeriesFLong",
    "PiFourMinusLn2HalfedSeriesArb",
    "PiFourMinusLn2HalfedSeriesCArb",
    "PiFourMinusLn2HalfedSeriesCF32",
    "PiFourMinusLn2HalfedSeriesCF64",
    "PiFourMinusLn2HalfedSeriesCFLong",
    "PiFourMinusLn2HalfedSeriesF32",
    "PiFourMinusLn2HalfedSeriesF64",
    "PiFourMinusLn2HalfedSeriesFLong",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesArb",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesCArb",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesCF32",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesCF64",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesCFLong",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesF32",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesF64",
    "PiMinus3pi4AndPiMinusXMinus3pi4SeriesFLong",
    "PiMinusX2SeriesArb",
    "PiMinusX2SeriesCArb",
    "PiMinusX2SeriesCF32",
    "PiMinusX2SeriesCF64",
    "PiMinusX2SeriesCFLong",
    "PiMinusX2SeriesF32",
    "PiMinusX2SeriesF64",
    "PiMinusX2SeriesFLong",
    "PiSeriesArb",
    "PiSeriesCArb",
    "PiSeriesCF32",
    "PiSeriesCF64",
    "PiSeriesCFLong",
    "PiSeriesF32",
    "PiSeriesF64",
    "PiSeriesFLong",
    "PiSixMinHalfSeriesArb",
    "PiSixMinHalfSeriesCArb",
    "PiSixMinHalfSeriesCF32",
    "PiSixMinHalfSeriesCF64",
    "PiSixMinHalfSeriesCFLong",
    "PiSixMinHalfSeriesF32",
    "PiSixMinHalfSeriesF64",
    "PiSixMinHalfSeriesFLong",
    "PiSquared6MinusOneSeriesArb",
    "PiSquared6MinusOneSeriesCArb",
    "PiSquared6MinusOneSeriesCF32",
    "PiSquared6MinusOneSeriesCF64",
    "PiSquared6MinusOneSeriesCFLong",
    "PiSquared6MinusOneSeriesF32",
    "PiSquared6MinusOneSeriesF64",
    "PiSquared6MinusOneSeriesFLong",
    "PiSquaredTwelveSeriesArb",
    "PiSquaredTwelveSeriesCArb",
    "PiSquaredTwelveSeriesCF32",
    "PiSquaredTwelveSeriesCF64",
    "PiSquaredTwelveSeriesCFLong",
    "PiSquaredTwelveSeriesF32",
    "PiSquaredTwelveSeriesF64",
    "PiSquaredTwelveSeriesFLong",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesArb",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCArb",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCF32",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCF64",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCFLong",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesF32",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesF64",
    "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesFLong",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesArb",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCArb",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCF32",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCF64",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCFLong",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesF32",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesF64",
    "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesFLong",
    "RemainderType",
    "RichardsonAlgorithmArb",
    "RichardsonAlgorithmCArb",
    "RichardsonAlgorithmCF32",
    "RichardsonAlgorithmCF64",
    "RichardsonAlgorithmCFLong",
    "RichardsonAlgorithmF32",
    "RichardsonAlgorithmF64",
    "RichardsonAlgorithmFLong",
    "RiemannZetaFuncSeriesArb",
    "RiemannZetaFuncSeriesCArb",
    "RiemannZetaFuncSeriesCF32",
    "RiemannZetaFuncSeriesCF64",
    "RiemannZetaFuncSeriesCFLong",
    "RiemannZetaFuncSeriesF32",
    "RiemannZetaFuncSeriesF64",
    "RiemannZetaFuncSeriesFLong",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesArb",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCArb",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCF32",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCF64",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCFLong",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesF32",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesF64",
    "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesFLong",
    "SeriesAccelerationArb",
    "SeriesAccelerationCArb",
    "SeriesAccelerationCF32",
    "SeriesAccelerationCF64",
    "SeriesAccelerationCFLong",
    "SeriesAccelerationF32",
    "SeriesAccelerationF64",
    "SeriesAccelerationFLong",
    "SeriesBaseArb",
    "SeriesBaseCArb",
    "SeriesBaseCF32",
    "SeriesBaseCF64",
    "SeriesBaseCFLong",
    "SeriesBaseF32",
    "SeriesBaseF64",
    "SeriesBaseFLong",
    "SeriesResultArb",
    "SeriesResultCArb",
    "SeriesResultCF32",
    "SeriesResultCF64",
    "SeriesResultCFLong",
    "SeriesResultF32",
    "SeriesResultF64",
    "SeriesResultFLong",
    "SeriesWithLnNumber1Arb",
    "SeriesWithLnNumber1CArb",
    "SeriesWithLnNumber1CF32",
    "SeriesWithLnNumber1CF64",
    "SeriesWithLnNumber1CFLong",
    "SeriesWithLnNumber1F32",
    "SeriesWithLnNumber1F64",
    "SeriesWithLnNumber1FLong",
    "SeriesWithLnNumber2Arb",
    "SeriesWithLnNumber2CArb",
    "SeriesWithLnNumber2CF32",
    "SeriesWithLnNumber2CF64",
    "SeriesWithLnNumber2CFLong",
    "SeriesWithLnNumber2F32",
    "SeriesWithLnNumber2F64",
    "SeriesWithLnNumber2FLong",
    "ShanksAlgorithmArb",
    "ShanksAlgorithmCArb",
    "ShanksAlgorithmCF32",
    "ShanksAlgorithmCF64",
    "ShanksAlgorithmCFLong",
    "ShanksAlgorithmF32",
    "ShanksAlgorithmF64",
    "ShanksAlgorithmFLong",
    "ShanksTransformAlternatingArb",
    "ShanksTransformAlternatingCArb",
    "ShanksTransformAlternatingCF32",
    "ShanksTransformAlternatingCF64",
    "ShanksTransformAlternatingCFLong",
    "ShanksTransformAlternatingF32",
    "ShanksTransformAlternatingF64",
    "ShanksTransformAlternatingFLong",
    "SiXSeriesArb",
    "SiXSeriesCArb",
    "SiXSeriesCF32",
    "SiXSeriesCF64",
    "SiXSeriesCFLong",
    "SiXSeriesF32",
    "SiXSeriesF64",
    "SiXSeriesFLong",
    "SinSeriesArb",
    "SinSeriesCArb",
    "SinSeriesCF32",
    "SinSeriesCF64",
    "SinSeriesCFLong",
    "SinSeriesF32",
    "SinSeriesF64",
    "SinSeriesFLong",
    "SinX2SeriesArb",
    "SinX2SeriesCArb",
    "SinX2SeriesCF32",
    "SinX2SeriesCF64",
    "SinX2SeriesCFLong",
    "SinX2SeriesF32",
    "SinX2SeriesF64",
    "SinX2SeriesFLong",
    "SinhSeriesArb",
    "SinhSeriesCArb",
    "SinhSeriesCF32",
    "SinhSeriesCF64",
    "SinhSeriesCFLong",
    "SinhSeriesF32",
    "SinhSeriesF64",
    "SinhSeriesFLong",
    "SinhX2SeriesArb",
    "SinhX2SeriesCArb",
    "SinhX2SeriesCF32",
    "SinhX2SeriesCF64",
    "SinhX2SeriesCFLong",
    "SinhX2SeriesF32",
    "SinhX2SeriesF64",
    "SinhX2SeriesFLong",
    "Sqrt1plusXMinus1MinusXDiv2SeriesArb",
    "Sqrt1plusXMinus1MinusXDiv2SeriesCArb",
    "Sqrt1plusXMinus1MinusXDiv2SeriesCF32",
    "Sqrt1plusXMinus1MinusXDiv2SeriesCF64",
    "Sqrt1plusXMinus1MinusXDiv2SeriesCFLong",
    "Sqrt1plusXMinus1MinusXDiv2SeriesF32",
    "Sqrt1plusXMinus1MinusXDiv2SeriesF64",
    "Sqrt1plusXMinus1MinusXDiv2SeriesFLong",
    "Sqrt1plusXSeriesArb",
    "Sqrt1plusXSeriesCArb",
    "Sqrt1plusXSeriesCF32",
    "Sqrt1plusXSeriesCF64",
    "Sqrt1plusXSeriesCFLong",
    "Sqrt1plusXSeriesF32",
    "Sqrt1plusXSeriesF64",
    "Sqrt1plusXSeriesFLong",
    "SqrtOneminusSqrtoneminusxDivXSeriesArb",
    "SqrtOneminusSqrtoneminusxDivXSeriesCArb",
    "SqrtOneminusSqrtoneminusxDivXSeriesCF32",
    "SqrtOneminusSqrtoneminusxDivXSeriesCF64",
    "SqrtOneminusSqrtoneminusxDivXSeriesCFLong",
    "SqrtOneminusSqrtoneminusxDivXSeriesF32",
    "SqrtOneminusSqrtoneminusxDivXSeriesF64",
    "SqrtOneminusSqrtoneminusxDivXSeriesFLong",
    "TenMinusXSeriesArb",
    "TenMinusXSeriesCArb",
    "TenMinusXSeriesCF32",
    "TenMinusXSeriesCF64",
    "TenMinusXSeriesCFLong",
    "TenMinusXSeriesF32",
    "TenMinusXSeriesF64",
    "TenMinusXSeriesFLong",
    "ThreeMinusPiSeriesArb",
    "ThreeMinusPiSeriesCArb",
    "ThreeMinusPiSeriesCF32",
    "ThreeMinusPiSeriesCF64",
    "ThreeMinusPiSeriesCFLong",
    "ThreeMinusPiSeriesF32",
    "ThreeMinusPiSeriesF64",
    "ThreeMinusPiSeriesFLong",
    "TwoArcsinSquareXHalfedSeriesArb",
    "TwoArcsinSquareXHalfedSeriesCArb",
    "TwoArcsinSquareXHalfedSeriesCF32",
    "TwoArcsinSquareXHalfedSeriesCF64",
    "TwoArcsinSquareXHalfedSeriesCFLong",
    "TwoArcsinSquareXHalfedSeriesF32",
    "TwoArcsinSquareXHalfedSeriesF64",
    "TwoArcsinSquareXHalfedSeriesFLong",
    "TwoDegreeXSeriesArb",
    "TwoDegreeXSeriesCArb",
    "TwoDegreeXSeriesCF32",
    "TwoDegreeXSeriesCF64",
    "TwoDegreeXSeriesCFLong",
    "TwoDegreeXSeriesF32",
    "TwoDegreeXSeriesF64",
    "TwoDegreeXSeriesFLong",
    "TwoLn2SeriesArb",
    "TwoLn2SeriesCArb",
    "TwoLn2SeriesCF32",
    "TwoLn2SeriesCF64",
    "TwoLn2SeriesCFLong",
    "TwoLn2SeriesF32",
    "TwoLn2SeriesF64",
    "TwoLn2SeriesFLong",
    "WenigerAlgorithmArb",
    "WenigerAlgorithmCArb",
    "WenigerAlgorithmCF32",
    "WenigerAlgorithmCF64",
    "WenigerAlgorithmCFLong",
    "WenigerAlgorithmF32",
    "WenigerAlgorithmF64",
    "WenigerAlgorithmFLong",
    "WynnEpsilon1AlgorithmArb",
    "WynnEpsilon1AlgorithmCArb",
    "WynnEpsilon1AlgorithmCF32",
    "WynnEpsilon1AlgorithmCF64",
    "WynnEpsilon1AlgorithmCFLong",
    "WynnEpsilon1AlgorithmF32",
    "WynnEpsilon1AlgorithmF64",
    "WynnEpsilon1AlgorithmFLong",
    "WynnEpsilon2AlgorithmArb",
    "WynnEpsilon2AlgorithmCArb",
    "WynnEpsilon2AlgorithmCF32",
    "WynnEpsilon2AlgorithmCF64",
    "WynnEpsilon2AlgorithmCFLong",
    "WynnEpsilon2AlgorithmF32",
    "WynnEpsilon2AlgorithmF64",
    "WynnEpsilon2AlgorithmFLong",
    "WynnEpsilon3AlgorithmArb",
    "WynnEpsilon3AlgorithmCArb",
    "WynnEpsilon3AlgorithmCF32",
    "WynnEpsilon3AlgorithmCF64",
    "WynnEpsilon3AlgorithmCFLong",
    "WynnEpsilon3AlgorithmF32",
    "WynnEpsilon3AlgorithmF64",
    "WynnEpsilon3AlgorithmFLong",
    "WynnRhoAlgorithmArb",
    "WynnRhoAlgorithmCArb",
    "WynnRhoAlgorithmCF32",
    "WynnRhoAlgorithmCF64",
    "WynnRhoAlgorithmCFLong",
    "WynnRhoAlgorithmF32",
    "WynnRhoAlgorithmF64",
    "WynnRhoAlgorithmFLong",
    "X1mxSquaredSeriesArb",
    "X1mxSquaredSeriesCArb",
    "X1mxSquaredSeriesCF32",
    "X1mxSquaredSeriesCF64",
    "X1mxSquaredSeriesCFLong",
    "X1mxSquaredSeriesF32",
    "X1mxSquaredSeriesF64",
    "X1mxSquaredSeriesFLong",
    "XDiv1minX2SeriesArb",
    "XDiv1minX2SeriesCArb",
    "XDiv1minX2SeriesCF32",
    "XDiv1minX2SeriesCF64",
    "XDiv1minX2SeriesCFLong",
    "XDiv1minX2SeriesF32",
    "XDiv1minX2SeriesF64",
    "XDiv1minX2SeriesFLong",
    "XDiv1minXSeriesArb",
    "XDiv1minXSeriesCArb",
    "XDiv1minXSeriesCF32",
    "XDiv1minXSeriesCF64",
    "XDiv1minXSeriesCFLong",
    "XDiv1minXSeriesF32",
    "XDiv1minXSeriesF64",
    "XDiv1minXSeriesFLong",
    "XMinSqrtXSeriesArb",
    "XMinSqrtXSeriesCArb",
    "XMinSqrtXSeriesCF32",
    "XMinSqrtXSeriesCF64",
    "XMinSqrtXSeriesCFLong",
    "XMinSqrtXSeriesF32",
    "XMinSqrtXSeriesF64",
    "XMinSqrtXSeriesFLong",
    "XSeriesArb",
    "XSeriesCArb",
    "XSeriesCF32",
    "XSeriesCF64",
    "XSeriesCFLong",
    "XSeriesF32",
    "XSeriesF64",
    "XSeriesFLong",
    "XTwoSeriesArb",
    "XTwoSeriesCArb",
    "XTwoSeriesCF32",
    "XTwoSeriesCF64",
    "XTwoSeriesCFLong",
    "XTwoSeriesF32",
    "XTwoSeriesF64",
    "XTwoSeriesFLong",
    "XTwoThroughtSquaresSeriesArb",
    "XTwoThroughtSquaresSeriesCArb",
    "XTwoThroughtSquaresSeriesCF32",
    "XTwoThroughtSquaresSeriesCF64",
    "XTwoThroughtSquaresSeriesCFLong",
    "XTwoThroughtSquaresSeriesF32",
    "XTwoThroughtSquaresSeriesF64",
    "XTwoThroughtSquaresSeriesFLong",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesArb",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesCArb",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesCF32",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesCF64",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesCFLong",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesF32",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesF64",
    "Xsquareplus3DivXsquareplus2multixMinus1SeriesFLong",
    "gamma_rho_type",
    "generalized_type",
    "rho_type",
    "t_type",
    "t_wave_type",
    "u_type",
    "v_type",
    "v_wave_type",
]

class AbsSinXMinus2DivPiSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class AbsSinXMinus2DivPiSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class AbsSinXMinus2DivPiSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class AbsSinXMinus2DivPiSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class AbsSinXMinus2DivPiSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class AbsSinXMinus2DivPiSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class AbsSinXMinus2DivPiSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class AbsSinXMinus2DivPiSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Arb:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> Arb: ...
    def __add__(self, arg0: Arb) -> Arb: ...
    def __eq__(self, arg0: Arb) -> bool: ...
    def __float__(self) -> float: ...
    def __format__(self, arg0: str) -> str: ...
    def __ge__(self, arg0: Arb) -> bool: ...
    def __getstate__(self) -> str: ...
    def __gt__(self, arg0: Arb) -> bool: ...
    def __index__(self) -> int: ...
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, d: float) -> None: ...
    @typing.overload
    def __init__(self, s: str) -> None: ...
    def __int__(self) -> int: ...
    def __le__(self, arg0: Arb) -> bool: ...
    def __lt__(self, arg0: Arb) -> bool: ...
    def __mul__(self, arg0: Arb) -> Arb: ...
    def __ne__(self, arg0: Arb) -> bool: ...
    def __neg__(self) -> Arb: ...
    def __pos__(self) -> Arb: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, arg0: str) -> None: ...
    def __str__(self) -> str: ...
    def __sub__(self, arg0: Arb) -> Arb: ...
    def __truediv__(self, arg0: Arb) -> Arb: ...

class ArcsinX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArcsinX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArcsinX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArcsinX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArcsinX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArcsinX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArcsinX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArcsinX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArcsinXMinusXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArcsinXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArcsinXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArcsinXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArcsinXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArcsinXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArcsinXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArcsinXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArcsinXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArcsinhXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArcsinhXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArcsinhXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArcsinhXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArcsinhXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArcsinhXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArcsinhXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArcsinhXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArctanhX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArctanhX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArctanhX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArctanhX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArctanhX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArctanhX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArctanhX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArctanhX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArctanhXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArctanhXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArctanhXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArctanhXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArctanhXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArctanhXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArctanhXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArctanhXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArctgX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArctgX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArctgX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArctgX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArctgX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArctgX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArctgX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArctgX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArctgX3SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArctgX3SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArctgX3SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArctgX3SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArctgX3SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArctgX3SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArctgX3SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArctgX3SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ArctgXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ArctgXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ArctgXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ArctgXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ArctgXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ArctgXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ArctgXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ArctgXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class BinSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class BinSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class BinSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class BinSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class BinSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class BinSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class BinSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class BinSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class BrezinskiThetaAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class CArb:
    __hash__: typing.ClassVar[None] = None
    imag: Arb
    real: Arb
    def __abs__(self) -> Arb: ...
    def __add__(self, arg0: CArb) -> CArb: ...
    def __complex__(self) -> ...: ...
    def __eq__(self, arg0: CArb) -> bool: ...
    def __format__(self, arg0: str) -> str: ...
    def __getstate__(self) -> tuple[str, str]: ...
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, re: Arb) -> None: ...
    @typing.overload
    def __init__(self, re: Arb, im: Arb) -> None: ...
    @typing.overload
    def __init__(self, z: ...) -> None: ...
    def __mul__(self, arg0: CArb) -> CArb: ...
    def __ne__(self, arg0: CArb) -> bool: ...
    def __neg__(self) -> CArb: ...
    def __pos__(self) -> CArb: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, arg0: tuple[str, str]) -> None: ...
    def __str__(self) -> str: ...
    def __sub__(self, arg0: CArb) -> CArb: ...
    def __truediv__(self, arg0: CArb) -> CArb: ...

class CF32:
    __hash__: typing.ClassVar[None] = None
    imag: float
    real: float
    def __abs__(self) -> float: ...
    def __add__(self, arg0: CF32) -> CF32: ...
    def __complex__(self) -> ...: ...
    def __eq__(self, arg0: CF32) -> bool: ...
    def __format__(self, arg0: str) -> str: ...
    def __getstate__(self) -> tuple[float, float]: ...
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, re: float) -> None: ...
    @typing.overload
    def __init__(self, re: float, im: float) -> None: ...
    @typing.overload
    def __init__(self, z: ...) -> None: ...
    def __mul__(self, arg0: CF32) -> CF32: ...
    def __ne__(self, arg0: CF32) -> bool: ...
    def __neg__(self) -> CF32: ...
    def __pos__(self) -> CF32: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, arg0: tuple[float, float]) -> None: ...
    def __str__(self) -> str: ...
    def __sub__(self, arg0: CF32) -> CF32: ...
    def __truediv__(self, arg0: CF32) -> CF32: ...

class CF64:
    __hash__: typing.ClassVar[None] = None
    imag: float
    real: float
    def __abs__(self) -> float: ...
    def __add__(self, arg0: CF64) -> CF64: ...
    def __complex__(self) -> ...: ...
    def __eq__(self, arg0: CF64) -> bool: ...
    def __format__(self, arg0: str) -> str: ...
    def __getstate__(self) -> tuple[float, float]: ...
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, re: float) -> None: ...
    @typing.overload
    def __init__(self, re: float, im: float) -> None: ...
    @typing.overload
    def __init__(self, z: ...) -> None: ...
    def __mul__(self, arg0: CF64) -> CF64: ...
    def __ne__(self, arg0: CF64) -> bool: ...
    def __neg__(self) -> CF64: ...
    def __pos__(self) -> CF64: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, arg0: tuple[float, float]) -> None: ...
    def __str__(self) -> str: ...
    def __sub__(self, arg0: CF64) -> CF64: ...
    def __truediv__(self, arg0: CF64) -> CF64: ...

class CFLong:
    __hash__: typing.ClassVar[None] = None
    imag: float
    real: float
    def __abs__(self) -> float: ...
    def __add__(self, arg0: CFLong) -> CFLong: ...
    def __complex__(self) -> ...: ...
    def __eq__(self, arg0: CFLong) -> bool: ...
    def __format__(self, arg0: str) -> str: ...
    def __getstate__(self) -> tuple[float, float]: ...
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, re: float) -> None: ...
    @typing.overload
    def __init__(self, re: float, im: float) -> None: ...
    @typing.overload
    def __init__(self, z: ...) -> None: ...
    def __mul__(self, arg0: CFLong) -> CFLong: ...
    def __ne__(self, arg0: CFLong) -> bool: ...
    def __neg__(self) -> CFLong: ...
    def __pos__(self) -> CFLong: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, arg0: tuple[float, float]) -> None: ...
    def __str__(self) -> str: ...
    def __sub__(self, arg0: CFLong) -> CFLong: ...
    def __truediv__(self, arg0: CFLong) -> CFLong: ...

class ChangWynnAlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class ChangWynnAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class ChangWynnAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class ChangWynnAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class ChangWynnAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class ChangWynnAlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class ChangWynnAlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class ChangWynnAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class CiXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class CiXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class CiXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class CiXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class CiXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class CiXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class CiXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class CiXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Cos3xMinus1DivXsquareSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class CosSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class CosSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class CosSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class CosSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class CosSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class CosSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class CosSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class CosSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class CosSqrtXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class CosSqrtXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class CosSqrtXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class CosSqrtXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class CosSqrtXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class CosSqrtXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class CosSqrtXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class CosSqrtXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class CosX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class CosX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class CosX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class CosX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class CosX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class CosX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class CosX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class CosX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class CoshSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class CoshSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class CoshSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class CoshSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class CoshSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class CoshSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class CoshSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class CoshSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class DrummondDAlgorithmArb(SeriesAccelerationArb):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class DrummondDAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class DrummondDAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class DrummondDAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class DrummondDAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class DrummondDAlgorithmF32(SeriesAccelerationF32):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class DrummondDAlgorithmF64(SeriesAccelerationF64):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class DrummondDAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(
        self, remainder: RemainderType = ..., useRecurrentFormula: bool = False
    ) -> None: ...

class EXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class EXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class EXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class EXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class EXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class EXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class EXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class EXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class EighthPiMOneThirdSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ErfSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ErfSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ErfSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ErfSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ErfSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ErfSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ErfSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ErfSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ExpMCosXSinsinXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ExpSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ExpSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ExpSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ExpSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ExpSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ExpSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ExpSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ExpSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ExpSquaredErfSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class FivePiTwelveSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class FordSidi2AlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class FordSidi3AlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class FourArctanSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class FourArctanSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class FourArctanSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class FourArctanSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class FourArctanSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class FourArctanSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class FourArctanSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class FourArctanSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class FourLn2M3SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class FourLn2M3SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class FourLn2M3SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class FourLn2M3SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class FourLn2M3SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class FourLn2M3SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class FourLn2M3SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class FourLn2M3SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class HalfAsinTwoXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class HalfMinusSinxMultiPi4SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class HalfMultiLn1Div2Multi1MinusCosxSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class IncompleteGammaFuncSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class InvSqrt1m4xSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Inverse1mxSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Inverse1mxSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Inverse1mxSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Inverse1mxSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Inverse1mxSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Inverse1mxSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Inverse1mxSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Inverse1mxSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class KXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class KXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class KXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class KXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class KXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class KXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class KXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class KXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class LambertWFuncSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class LambertWFuncSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class LambertWFuncSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class LambertWFuncSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class LambertWFuncSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class LambertWFuncSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class LambertWFuncSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class LambertWFuncSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class LevinAlgorithmArb(SeriesAccelerationArb):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: Arb = ...,
    ) -> None: ...

class LevinAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: Arb = ...,
    ) -> None: ...

class LevinAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinAlgorithmF32(SeriesAccelerationF32):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinAlgorithmF64(SeriesAccelerationF64):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinSidiMAlgorithmArb(SeriesAccelerationArb):
    def __init__(self, remainder: RemainderType = ..., gamma: Arb = ...) -> None: ...

class LevinSidiMAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self, remainder: RemainderType = ..., gamma: Arb = ...) -> None: ...

class LevinSidiMAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self, remainder: RemainderType = ..., gamma: float = 10.0) -> None: ...

class LevinSidiMAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self, remainder: RemainderType = ..., gamma: float = 10.0) -> None: ...

class LevinSidiMAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self, remainder: RemainderType = ..., gamma: float = 10.0) -> None: ...

class LevinSidiMAlgorithmF32(SeriesAccelerationF32):
    def __init__(self, remainder: RemainderType = ..., gamma: float = 10.0) -> None: ...

class LevinSidiMAlgorithmF64(SeriesAccelerationF64):
    def __init__(self, remainder: RemainderType = ..., gamma: float = 10.0) -> None: ...

class LevinSidiMAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self, remainder: RemainderType = ..., gamma: float = 10.0) -> None: ...

class LevinSidiSAlgorithmArb(SeriesAccelerationArb):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: Arb = ...,
    ) -> None: ...

class LevinSidiSAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: Arb = ...,
    ) -> None: ...

class LevinSidiSAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinSidiSAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinSidiSAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinSidiSAlgorithmF32(SeriesAccelerationF32):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinSidiSAlgorithmF64(SeriesAccelerationF64):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class LevinSidiSAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(
        self,
        remainder: RemainderType = ...,
        useRecurrentFormula: bool = False,
        beta: float = 1.0,
    ) -> None: ...

class Ln13MinusLn7Div7SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln13MinusLn7Div7SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln13MinusLn7Div7SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln13MinusLn7Div7SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln13MinusLn7Div7SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln13MinusLn7Div7SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln13MinusLn7Div7SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln13MinusLn7Div7SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln1MinusX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln1PlusSqrt1PlusXsquareMinusLn2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln1PlusX3SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Ln1mxSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln1mxSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln1mxSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln1mxSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln1mxSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln1mxSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln1mxSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln1mxSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln1plusXDiv1plusX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Ln1px4SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln1px4SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln1px4SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln1px4SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln1px4SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln1px4SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln1px4SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln1px4SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Ln2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Ln2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Ln2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Ln2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Ln2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Ln2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Ln2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Ln2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class LnCosxSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class LnCosxSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class LnCosxSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class LnCosxSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class LnCosxSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class LnCosxSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class LnCosxSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class LnCosxSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class LnSinxMinusLnXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class LnXPlusOneXMinusOneHalfedSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class LubkinWAlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class LubkinWAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class LubkinWAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class LubkinWAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class LubkinWAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class LubkinWAlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class LubkinWAlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class LubkinWAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class MFact1mxMp1InverseSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class MeanSinhSinSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Minus3Div4OrXMinus3Div4SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class MinusOneNFactNInNSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class MinusOneNedInNSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class MinusOneQuarterSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class MinusXMinusPi4OrMinusPi4SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class NumeratorType:
    """
    Members:

      rho_type

      generalized_type

      gamma_rho_type
    """

    __members__: typing.ClassVar[
        dict[str, NumeratorType]
    ]  # value = {'rho_type': <NumeratorType.rho_type: 0>, 'generalized_type': <NumeratorType.generalized_type: 1>, 'gamma_rho_type': <NumeratorType.gamma_rho_type: 2>}
    gamma_rho_type: typing.ClassVar[
        NumeratorType
    ]  # value = <NumeratorType.gamma_rho_type: 2>
    generalized_type: typing.ClassVar[
        NumeratorType
    ]  # value = <NumeratorType.generalized_type: 1>
    rho_type: typing.ClassVar[NumeratorType]  # value = <NumeratorType.rho_type: 0>
    def __eq__(self, other: typing.Any) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __init__(self, value: int) -> None: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: typing.Any) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    def __str__(self) -> str: ...
    @property
    def name(self) -> str: ...
    @property
    def value(self) -> int: ...

class OneDivSqrt2SinXDivSqrt2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneDivSqrt2SinXDivSqrt2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneDivSqrt2SinXDivSqrt2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneDivSqrt2SinXDivSqrt2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneDivSqrt2SinXDivSqrt2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneDivSqrt2SinXDivSqrt2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneDivSqrt2SinXDivSqrt2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneDivSqrt2SinXDivSqrt2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneDivTwoMinusXMultiThreePlusXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneMinusSqrt1minus4xDiv2xSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class OneSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneThirdPiSquaredMNineSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneTwelfth3x2Pi2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class OneTwelfthSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneTwelfthSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneTwelfthSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneTwelfthSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneTwelfthSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneTwelfthSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneTwelfthSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneTwelfthSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class OneTwelfthX2Pi2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Pi3SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Pi3SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Pi3SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Pi3SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Pi3SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Pi3SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Pi3SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Pi3SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Pi4SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Pi4SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Pi4SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Pi4SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Pi4SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Pi4SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Pi4SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Pi4SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Pi8CosxSquareMinus1Div3CosxSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiCubed32SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiCubed32SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiCubed32SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiCubed32SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiCubed32SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiCubed32SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiCubed32SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiCubed32SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiFourMinusLn2HalfedSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiMinus3pi4AndPiMinusXMinus3pi4SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiMinusX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiMinusX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiMinusX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiMinusX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiMinusX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiMinusX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiMinusX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiMinusX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiSixMinHalfSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiSquared6MinusOneSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiSquaredTwelveSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class RemainderType:
    """
    Members:

      u_type

      t_type

      v_type

      t_wave_type

      v_wave_type
    """

    __members__: typing.ClassVar[
        dict[str, RemainderType]
    ]  # value = {'u_type': <RemainderType.u_type: 0>, 't_type': <RemainderType.t_type: 1>, 'v_type': <RemainderType.v_type: 2>, 't_wave_type': <RemainderType.t_wave_type: 3>, 'v_wave_type': <RemainderType.v_wave_type: 4>}
    t_type: typing.ClassVar[RemainderType]  # value = <RemainderType.t_type: 1>
    t_wave_type: typing.ClassVar[
        RemainderType
    ]  # value = <RemainderType.t_wave_type: 3>
    u_type: typing.ClassVar[RemainderType]  # value = <RemainderType.u_type: 0>
    v_type: typing.ClassVar[RemainderType]  # value = <RemainderType.v_type: 2>
    v_wave_type: typing.ClassVar[
        RemainderType
    ]  # value = <RemainderType.v_wave_type: 4>
    def __eq__(self, other: typing.Any) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __init__(self, value: int) -> None: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: typing.Any) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    def __str__(self) -> str: ...
    @property
    def name(self) -> str: ...
    @property
    def value(self) -> int: ...

class RichardsonAlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class RichardsonAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class RichardsonAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class RichardsonAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class RichardsonAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class RichardsonAlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class RichardsonAlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class RichardsonAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class RiemannZetaFuncSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class SeriesAccelerationArb:
    def __call__(self, n: int, order: int, data: SeriesResultArb) -> Arb: ...
    def printInfo(self) -> str: ...

class SeriesAccelerationCArb:
    def __call__(self, n: int, order: int, data: SeriesResultCArb) -> CArb: ...
    def printInfo(self) -> str: ...

class SeriesAccelerationCF32:
    def __call__(self, n: int, order: int, data: SeriesResultCF32) -> CF32: ...
    def printInfo(self) -> str: ...

class SeriesAccelerationCF64:
    def __call__(self, n: int, order: int, data: SeriesResultCF64) -> CF64: ...
    def printInfo(self) -> str: ...

class SeriesAccelerationCFLong:
    def __call__(self, n: int, order: int, data: SeriesResultCFLong) -> CFLong: ...
    def printInfo(self) -> str: ...

class SeriesAccelerationF32:
    def __call__(self, n: int, order: int, data: SeriesResultF32) -> float: ...
    def printInfo(self) -> str: ...

class SeriesAccelerationF64:
    def __call__(self, n: int, order: int, data: SeriesResultF64) -> float: ...
    def printInfo(self) -> str: ...

class SeriesAccelerationFLong:
    def __call__(self, n: int, order: int, data: SeriesResultFLong) -> float: ...
    def printInfo(self) -> str: ...

class SeriesBaseArb:
    def generateSeries(
        self, x: Arb, vecSize: int, addTParameter: Arb = ..., addKParameter: int = 1
    ) -> SeriesResultArb: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> Arb: ...
    def get_x(self) -> Arb: ...

class SeriesBaseCArb:
    def generateSeries(
        self, x: CArb, vecSize: int, addTParameter: CArb = ..., addKParameter: int = 1
    ) -> SeriesResultCArb: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> CArb: ...
    def get_x(self) -> CArb: ...

class SeriesBaseCF32:
    def generateSeries(
        self, x: CF32, vecSize: int, addTParameter: CF32 = ..., addKParameter: int = 1
    ) -> SeriesResultCF32: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> CF32: ...
    def get_x(self) -> CF32: ...

class SeriesBaseCF64:
    def generateSeries(
        self, x: CF64, vecSize: int, addTParameter: CF64 = ..., addKParameter: int = 1
    ) -> SeriesResultCF64: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> CF64: ...
    def get_x(self) -> CF64: ...

class SeriesBaseCFLong:
    def generateSeries(
        self,
        x: CFLong,
        vecSize: int,
        addTParameter: CFLong = ...,
        addKParameter: int = 1,
    ) -> SeriesResultCFLong: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> CFLong: ...
    def get_x(self) -> CFLong: ...

class SeriesBaseF32:
    def generateSeries(
        self, x: float, vecSize: int, addTParameter: float = 1.0, addKParameter: int = 1
    ) -> SeriesResultF32: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> float: ...
    def get_x(self) -> float: ...

class SeriesBaseF64:
    def generateSeries(
        self, x: float, vecSize: int, addTParameter: float = 1.0, addKParameter: int = 1
    ) -> SeriesResultF64: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> float: ...
    def get_x(self) -> float: ...

class SeriesBaseFLong:
    def generateSeries(
        self, x: float, vecSize: int, addTParameter: float = 1.0, addKParameter: int = 1
    ) -> SeriesResultFLong: ...
    def get_name(self) -> str: ...
    def get_sum(self) -> float: ...
    def get_x(self) -> float: ...

class SeriesResultArb:
    Sn: list[Arb]
    an: list[Arb]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[Arb], arg1: list[Arb]) -> None: ...

class SeriesResultCArb:
    Sn: list[CArb]
    an: list[CArb]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[CArb], arg1: list[CArb]) -> None: ...

class SeriesResultCF32:
    Sn: list[CF32]
    an: list[CF32]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[CF32], arg1: list[CF32]) -> None: ...

class SeriesResultCF64:
    Sn: list[CF64]
    an: list[CF64]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[CF64], arg1: list[CF64]) -> None: ...

class SeriesResultCFLong:
    Sn: list[CFLong]
    an: list[CFLong]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[CFLong], arg1: list[CFLong]) -> None: ...

class SeriesResultF32:
    Sn: list[float]
    an: list[float]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[float], arg1: list[float]) -> None: ...

class SeriesResultF64:
    Sn: list[float]
    an: list[float]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[float], arg1: list[float]) -> None: ...

class SeriesResultFLong:
    Sn: list[float]
    an: list[float]
    @typing.overload
    def __init__(self) -> None: ...
    @typing.overload
    def __init__(self, arg0: list[float], arg1: list[float]) -> None: ...

class SeriesWithLnNumber1Arb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SeriesWithLnNumber1CArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SeriesWithLnNumber1CF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SeriesWithLnNumber1CF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SeriesWithLnNumber1CFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SeriesWithLnNumber1F32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SeriesWithLnNumber1F64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SeriesWithLnNumber1FLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2Arb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2CArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2CF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2CF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2CFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2F32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2F64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SeriesWithLnNumber2FLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ShanksAlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class ShanksAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class ShanksAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class ShanksAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class ShanksAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class ShanksAlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class ShanksAlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class ShanksAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class ShanksTransformAlternatingFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class SiXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SiXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SiXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SiXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SiXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SiXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SiXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SiXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class SinSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SinSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SinSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SinSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SinSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SinSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SinSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SinSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class SinX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SinX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SinX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SinX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SinX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SinX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SinX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SinX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class SinhSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SinhSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SinhSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SinhSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SinhSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SinhSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SinhSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SinhSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class SinhX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SinhX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SinhX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SinhX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SinhX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SinhX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SinhX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SinhX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Sqrt1plusXMinus1MinusXDiv2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Sqrt1plusXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class SqrtOneminusSqrtoneminusxDivXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class TenMinusXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class TenMinusXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class TenMinusXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class TenMinusXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class TenMinusXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class TenMinusXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class TenMinusXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class TenMinusXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class ThreeMinusPiSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class TwoArcsinSquareXHalfedSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class TwoDegreeXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class TwoLn2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class TwoLn2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class TwoLn2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class TwoLn2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class TwoLn2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class TwoLn2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class TwoLn2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class TwoLn2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class WenigerAlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class WenigerAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class WenigerAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class WenigerAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class WenigerAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class WenigerAlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class WenigerAlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class WenigerAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class WynnEpsilon1AlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmArb(SeriesAccelerationArb):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmF32(SeriesAccelerationF32):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmF64(SeriesAccelerationF64):
    def __init__(self) -> None: ...

class WynnEpsilon2AlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self) -> None: ...

class WynnEpsilon3AlgorithmArb(SeriesAccelerationArb):
    def __init__(self, epsilon_threshold: Arb = ...) -> None: ...

class WynnEpsilon3AlgorithmCArb(SeriesAccelerationCArb):
    def __init__(self, epsilon_threshold: Arb = ...) -> None: ...

class WynnEpsilon3AlgorithmCF32(SeriesAccelerationCF32):
    def __init__(self, epsilon_threshold: float = 0.0010000000474974513) -> None: ...

class WynnEpsilon3AlgorithmCF64(SeriesAccelerationCF64):
    def __init__(self, epsilon_threshold: float = 0.001) -> None: ...

class WynnEpsilon3AlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(self, epsilon_threshold: float = 0.001) -> None: ...

class WynnEpsilon3AlgorithmF32(SeriesAccelerationF32):
    def __init__(self, epsilon_threshold: float = 0.0010000000474974513) -> None: ...

class WynnEpsilon3AlgorithmF64(SeriesAccelerationF64):
    def __init__(self, epsilon_threshold: float = 0.001) -> None: ...

class WynnEpsilon3AlgorithmFLong(SeriesAccelerationFLong):
    def __init__(self, epsilon_threshold: float = 0.001) -> None: ...

class WynnRhoAlgorithmArb(SeriesAccelerationArb):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: Arb = ..., rho: Arb = ...
    ) -> None: ...

class WynnRhoAlgorithmCArb(SeriesAccelerationCArb):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: Arb = ..., rho: Arb = ...
    ) -> None: ...

class WynnRhoAlgorithmCF32(SeriesAccelerationCF32):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: float = -1.0, rho: float = 1.0
    ) -> None: ...

class WynnRhoAlgorithmCF64(SeriesAccelerationCF64):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: float = -1.0, rho: float = 1.0
    ) -> None: ...

class WynnRhoAlgorithmCFLong(SeriesAccelerationCFLong):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: float = -1.0, rho: float = 1.0
    ) -> None: ...

class WynnRhoAlgorithmF32(SeriesAccelerationF32):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: float = -1.0, rho: float = 1.0
    ) -> None: ...

class WynnRhoAlgorithmF64(SeriesAccelerationF64):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: float = -1.0, rho: float = 1.0
    ) -> None: ...

class WynnRhoAlgorithmFLong(SeriesAccelerationFLong):
    def __init__(
        self, numerator: NumeratorType = ..., gamma: float = -1.0, rho: float = 1.0
    ) -> None: ...

class X1mxSquaredSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class X1mxSquaredSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class X1mxSquaredSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class X1mxSquaredSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class X1mxSquaredSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class X1mxSquaredSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class X1mxSquaredSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class X1mxSquaredSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class XDiv1minX2SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class XDiv1minXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class XDiv1minXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class XDiv1minXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class XDiv1minXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class XDiv1minXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class XDiv1minXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class XDiv1minXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class XDiv1minXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class XMinSqrtXSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class XSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class XSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class XSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class XSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class XSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class XSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class XSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class XSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class XTwoSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class XTwoSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class XTwoSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class XTwoSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class XTwoSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class XTwoSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class XTwoSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class XTwoSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class XTwoThroughtSquaresSeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesArb(SeriesBaseArb):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesCArb(SeriesBaseCArb):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesCF32(SeriesBaseCF32):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesCF64(SeriesBaseCF64):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesCFLong(SeriesBaseCFLong):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesF32(SeriesBaseF32):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesF64(SeriesBaseF64):
    def __init__(self) -> None: ...

class Xsquareplus3DivXsquareplus2multixMinus1SeriesFLong(SeriesBaseFLong):
    def __init__(self) -> None: ...

gamma_rho_type: NumeratorType  # value = <NumeratorType.gamma_rho_type: 2>
generalized_type: NumeratorType  # value = <NumeratorType.generalized_type: 1>
rho_type: NumeratorType  # value = <NumeratorType.rho_type: 0>
t_type: RemainderType  # value = <RemainderType.t_type: 1>
t_wave_type: RemainderType  # value = <RemainderType.t_wave_type: 3>
u_type: RemainderType  # value = <RemainderType.u_type: 0>
v_type: RemainderType  # value = <RemainderType.v_type: 2>
v_wave_type: RemainderType  # value = <RemainderType.v_wave_type: 4>
