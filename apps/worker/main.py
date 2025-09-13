from shanks_consumer import params

def main():
    cfg = params.get_series_params_from_json("example.json")
    print(cfg)

if __name__ == "__main__":
    main()
