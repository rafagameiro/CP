
import java.lang.Math;
import java.util.Random;

public class piSimulation {

    private int simulations;
    private double pi;
    private int pointsCircle;

    public piSimulation(int simulations) {
        this.simulations = simulations;
        this.pi = -1;
    }

    public double getPi() {
        
        if(pi == -1) {
            pi = (pointsCircle / (float)simulations) * 4;
        }

        return pi;
    }

    public int getPoints() {
        return pointsCircle;
    }

    
    public void simulateParalel() {

        for(int i = 0; i < simulations; i++) {
            doSimulation();
        }

    }

    public void simulate() {
    
        for(int i = 0; i < simulations; i++) {
            doSimulation();
        }

    }

    private void doSimulation() {

        Random rand = new Random();
        double x = -1;
        double y = -1;
        double calc = -1;

        x = rand.nextDouble();
        y = rand.nextDouble();

        calc = Math.pow(x, 2) + Math.pow(y, 2);

        if(calc <= 1.0)
            pointsCircle++;
    }


}
