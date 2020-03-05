

public class approxPI {


    public static void main(String[] args) {

        if(args.length != 1) {
            System.err.println("You need to say the number of simulations.");
            System.exit(1);
        }

        piSimulation ps = new piSimulation(Integer.parseInt(args[0]));
        //Simulate sequentially, without threads
        //ps.simulate();

        //Simulate with threads
        //ps.simulateParalel();

        System.out.println("Total number of points: " + args[0]);
        System.out.println("Points within circle: \t" + ps.getPoints());
        System.out.println("Pi estimation: \t\t" + ps.getPi());

    }

}
