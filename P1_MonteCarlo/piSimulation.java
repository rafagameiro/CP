
import java.lang.Math;
import java.util.Random;
import java.util.List;
import java.util.LinkedList;
import java.util.concurrent.Callable; 
import java.util.concurrent.FutureTask; 

import java.lang.InterruptedException;
import java.util.concurrent.ExecutionException;

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

    
    public void simulateParalel(int nThreads) {
        List<FutureTask> threads = new LinkedList<FutureTask>();

        try {
            int simulPerThread = simulations/nThreads;

            for(int i = 0; i < nThreads; i++) {

                Callable callable = new threadSimulation(simulPerThread);

                threads.add(new FutureTask(callable));

                Thread t = new Thread(threads.get(i));
                t.start();
            }


            for(FutureTask task: threads) {
                pointsCircle += (int)task.get();
            }

        } catch(ExecutionException e) {
            System.err.println("An Error occorred during threads execution.");
        } catch(InterruptedException e) {
            System.err.println("An Error occorred during threads execution.");
        }
    }

    public void simulate() {
    
        for(int i = 0; i < simulations; i++) {
            doSimulation();
        }

    }

    private void doSimulation() {

        Random rand = new Random();
        double x = rand.nextDouble();
        double y = rand.nextDouble();
        double calc;

        calc = Math.pow(x, 2) + Math.pow(y, 2);

        if(calc <= 1.0)
            pointsCircle++;
    }


}
