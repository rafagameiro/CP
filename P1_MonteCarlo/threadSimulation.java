
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.Callable; 

public class threadSimulation implements Callable {

    private int simulations;
    private int points;

    public threadSimulation(int simulations) {
        this.simulations = simulations;
    }

    public Object call() {

        for(int i = 0; i < simulations; i++)
            doSimulation();

        return points;
    }

     private void doSimulation() {

        double x = ThreadLocalRandom.current().nextDouble();
        double y = ThreadLocalRandom.current().nextDouble();
        double calc;

        calc = Math.pow(x, 2) + Math.pow(y, 2);

        if(calc <= 1.0)
            points++;

    }

}
