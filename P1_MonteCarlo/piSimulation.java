
import java.lang.Math;
import java.util.Random;
import java.util.List;
import java.util.LinkedList;
import java.util.concurrent.*; 

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

        ExecutorService service = Executors.newFixedThreadPool(nThreads);
        List<Future> threads = new LinkedList<Future>();

        try {
            int simulPerThread = simulations/nThreads;

            for(int i = 0; i < nThreads; i++) {
                Future<Integer> future = service.submit(new threadSimulation(simulPerThread));
                threads.add(future);
            }


            for(Future task: threads) {
                pointsCircle += (int) task.get();
            }

        } catch(ExecutionException e) {
            System.err.println("An Error occorred during threads execution.");
        } catch(InterruptedException e) {
            System.err.println("An Error occorred during threads execution.");
        }

        service.shutdown();
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
