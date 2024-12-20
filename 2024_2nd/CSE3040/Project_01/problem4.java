package homework1;

public class problem4 {
	public static void main(String[] args) {
		 String input = String.join(" ", args);
		
		StringBuilder reversedString = new StringBuilder(input);
        System.out.println(reversedString.reverse().toString());
	}
}
