package ecoSpace;

public class NubKeyQueryService implements QueryService {
	private String query;
	public NubKeyQueryService(String query) {
		this.query=query;
	}

	@Override
	public String[] executeQuery() {
		return DatasetIndex.getSpeciesFromNub(this.query.split(","));
	}
}
