package cp.articlerep;

import java.util.Random;

import cp.articlerep.ds.HashTable;
import cp.articlerep.ds.Map;

public class Worker extends Thread {

	private static final int TABLE_SIZE = 20000;
	
	private final static Map<Integer,String> sharedMap = new HashTable<Integer,String>(TABLE_SIZE);
	
	
	private final Random rand = new Random();
	private final String[] sharedDict;
	private final StopVar sharedStop;
	
	private int numOps = 0;
	private int succPutOps = 0;
	private int succRemOps = 0;
	private int succGetOps = 0;
	
	public Worker(String[] sharedDict, StopVar sharedStop) {
		super();
		this.sharedDict = sharedDict;
		this.sharedStop = sharedStop;
	}
	
	@Override
	public void run() {
		while(!sharedStop.stop) {
			int op = rand.nextInt(100);
			int id = rand.nextInt(sharedDict.length);
			if (op < 25) {
				// probability 25% of trying to insert 'id'
				if (sharedMap.put(id, sharedDict[id]) == null) {
					succPutOps++;
					if (Main.SANITY_CHECK) {
						Main.addedIds.add(id);
					}
                                        
                                    if(sharedMap.get(id) == null || !Main.addedIds.contains(id)) {
                                        System.out.println("The newly inserted Id is not in both map and set!");
                                        System.exit(1);
                                    }
				}
			}
			else if (op < 50) {
				// probability 25% of trying to remove 'id'
				if (sharedMap.remove(id) != null) {
					succRemOps++;
					if (Main.SANITY_CHECK) {
						Main.addedIds.remove(id);
					}
				}

                                if(sharedMap.get(id) != null || Main.addedIds.contains(id)) {
                                        System.out.println("The newly removed Id is exists in one of data structures!");
                                        System.exit(1);
                                }
			}
			else {
				// probability 50% of trying to lookup for 'id'
				if (sharedMap.get(id) != null) {
					succGetOps++;
				}
			}
			numOps++;
		}
	}
	
	public int getNumOps() {
		return numOps;
	}

	public int getSuccPutOps() {
		return succPutOps;
	}

	public int getSuccRemOps() {
		return succRemOps;
	}

	public int getSuccGetOps() {
		return succGetOps;
	}

	public static Map<Integer,String> getSharedMap() {
		return sharedMap;
	}
}
