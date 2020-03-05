

public class approxPIPar {


    public static void main(String[] args) {

        if(args.length != 2) {
            System.err.println("You need to say the number of simulations and threads.");
            System.exit(1);
        }

        piSimulation ps = new piSimulation(Integer.parseInt(args[0]));
        ps.simulateParalel(Integer.parseInt(args[1]));

        System.out.println("Total number of points: " + args[0]);
        System.out.println("Points within circle: \t" + ps.getPoints());
        System.out.println("Pi estimation: \t\t" + ps.getPi());

    }

}
