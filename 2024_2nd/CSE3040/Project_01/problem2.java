package homework1;

public class problem2 {
	public static void main(String[] args) {	
		for (int i = 0; i < 17; i++) {
			for (int j = 0; j < 17; j++) {
				if (j == i || j == 16 - i) {
					if (j == 16) {
						System.out.println("*");
					} else {
						System.out.print("*");
					}
				} else {
					if (j == 16) {
						System.out.println(" ");
					} else {
						System.out.print(" ");
					}
				}
			}
		}
	}
}
