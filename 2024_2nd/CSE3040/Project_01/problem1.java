package homework1;
import java.util.Random;

public class problem1 {
	public static void main(String[] args) {
		Random rand = new Random();
        
        while(true) {
            int die1 = rand.nextInt(6) + 1;
            int die2 = rand.nextInt(6) + 1;
            
            System.out.print("(" + die1 + "," + die2 + ")");
            
            if (die1 + die2 == 7) {
            	break;
            } else {
                System.out.print(" ");
            }
        } 
	}
}
