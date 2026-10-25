"""Probe every registered C++ series in a separate process.

This detects crashes, unsupported sums and gross discrepancies at sample points.
A small residual is NOT an independent mathematical verification. Slow series,
asymptotic expansions and sequence generators require individual interpretation.
Usage: python tools/probe_cpp_catalogue.py /tmp/shanks-catalogue-probe --output report.json
"""
import argparse
import json
import math
from pathlib import Path
import re
import subprocess


def probe(binary, root, count=256):
    registry=root/'backend/core/include/series_registry.def'
    names=re.findall(r'^SERIES_(?:ENTRY(?:_ARGS)?|LAST)\((\w+),',registry.read_text(),re.MULTILINE)
    results=[]
    for name in names:
        x='3' if name.startswith('riemann_zeta') else ('10' if name=='ten_minus_x' else ('0' if name.startswith('rump_seq') else '0.05'))
        row={'name':name,'x':x,'count':count}
        try:
            run=subprocess.run([str(binary),name,x,str(count)],capture_output=True,text=True,timeout=10)
            row['exit_code']=run.returncode
            if run.returncode:
                row.update(status='process_error',message=run.stderr.strip()[:1000])
            else:
                values=[float(v) for v in run.stdout.split()]
                expected,observed,*terms=values
                row.update(reference=str(expected),observed=str(observed),first_terms=[str(t) for t in terms])
                if not math.isfinite(expected) and math.isfinite(observed):
                    row['status']='reference_unavailable'
                elif not math.isfinite(expected) or not math.isfinite(observed):
                    row['status']='nonfinite'
                else:
                    row['relative_residual']=abs(observed-expected)/max(1,abs(expected))
                    row['status']='sample_agrees' if row['relative_residual']<1e-6 else 'review_residual'
        except subprocess.TimeoutExpired:
            row['status']='timeout'
        results.append(row)
    return {'note':'Exploratory floating-point samples; sample_agrees does not certify a formula.','series':results}


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('binary',type=Path)
    parser.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    data=probe(args.binary,args.source)
    text=json.dumps(data,indent=2,allow_nan=False)+'\n'
    if args.output:
        args.output.write_text(text)
    else:
        print(text,end='')
