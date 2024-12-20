package homework1;

public class problem3 {
	public static void main(String[] args) {
		int num = Integer.parseInt(args[0]);
		
		if (num < 1 || num > 9) {
            System.out.println("입력값은 1에서 9 사이여야 합니다.");
            
            return;
        }
		
		for (int i = 1; i <= 9; i++) {
	            System.out.println(num + " × " + i + " = " + (num * i));
	    }
	}
}
