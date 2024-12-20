package homework2;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class calculator_20191282 extends Frame {
    Panel topPanel;
    TextField tf;
    Panel buttonPanel;
    Button[] buttons;
    Font font;
    Font buttonFont;
    String currentStr;

    public calculator_20191282(String title) {
        super(title);
        setSize(700, 400);
        setLocation(300, 300);

        topPanel = new Panel();
        topPanel.setLayout(new BorderLayout());

        font = new Font("Dialog", Font.PLAIN, 40);
        currentStr = "0";
        tf = new TextField(currentStr, 100);
        tf.setEditable(false);
        tf.setPreferredSize(new Dimension(500, 100));
        tf.setFont(font);
        topPanel.add(tf, "Center");

        buttonPanel = new Panel();
        buttonPanel.setLayout(new GridLayout(5, 5, 4, 4));
        buttonPanel.setBackground(Color.white);
        add(topPanel, "North");
        add(buttonPanel, "Center");

        String numStr[] = {
                "", "x!", "(", ")", "%", "AC",
                "sin", "ln", "7", "8", "9", "÷",
                "cos", "log", "4", "5", "6", "×",
                "tan", "root", "1", "2", "3", "-",
                "", "x^y", "0", ".", "=", "+"
        };

        buttons = new Button[numStr.length];
        buttonFont = new Font("Dialog", Font.PLAIN, 17);
        
        for (int i = 0; i < 30; i++) {
            buttons[i] = new Button(numStr[i]);
            buttons[i].setForeground(Color.blue);
            buttons[i].setFont(buttonFont);
            buttonPanel.add(buttons[i]);
        }

        addActionListeners();
        setResizable(false);
        setVisible(true);
        
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                System.exit(0);
            }
        });
    }

    public static void main(String[] args) {
        calculator_20191282 mainWindow = new calculator_20191282("Calculation");
    }
    
    public void addActionListeners() {
        for (Button button : buttons) {
            button.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    String command = e.getActionCommand();

                    if ("AC".equals(command)) {
                        currentStr = "0";
                        tf.setText(currentStr);
                    } else if ("=".equals(command)) {
                        currentStr = tf.getText();
                        String answer = getAnswer(currentStr);
                        tf.setText(answer);
                        currentStr = answer;
                    } else {
                        currentStr = tf.getText();

                        if (currentStr.equals("Error") || currentStr.equals("NaN")) {
                            currentStr = "";
                        }

                        if (currentStr.equals("0")) {
                            if (command.equals(".")) {
                                currentStr = "0.";
                            } else if (command.equals("x!")) {
                            	currentStr = "0" + "!";
                            } else if (command.equals("x^y")) {
                            	currentStr = "0" + "^";
                            } else if (!Character.isDigit(command.charAt(0)) && !command.equals("(") && !command.equals(")")) {
                                currentStr = "0" + command;
                            } else if (command.equals(")")) {
                                currentStr = "0";
                            } else {
                            	currentStr = command;
                            }
                        } else {
                            char lastChar = currentStr.isEmpty() ? '\0' : currentStr.charAt(currentStr.length() - 1);
                            char secondLastChar = currentStr.length() > 1 ? currentStr.charAt(currentStr.length() - 2) : '\0';

                            if (command.equals("%")) {
                                if (Character.isDigit(lastChar)) {
                                    currentStr += "%";
                                }
                            } else if (command.equals("x!")) {
                                if (Character.isDigit(lastChar)) {
                                    currentStr += "!";
                                }
                            } else if (command.equals("x^y")) {
                                if (Character.isDigit(lastChar)) {
                                    currentStr += "^";
                                }
                            } else if (command.equals("(")) {
                                if (isOperator(lastChar)) {
                                    currentStr += "(";
                                }
                            } else if (command.equals(")")) {
                                if (Character.isDigit(lastChar)) {
                                    currentStr += ")";
                                }
                            } else if (isOperator(lastChar)) {
                                if (lastChar == '-' && (secondLastChar == '×' || secondLastChar == '÷' || lastChar == '^')) {
                                    if (Character.isDigit(command.charAt(0))) {
                                        currentStr += command;
                                    } else if (isOperator(command.charAt(0))) {
                                        currentStr = currentStr.substring(0, currentStr.length() - 2) + command;
                                    }
                                } else if (command.equals("-") && (lastChar == '×' || lastChar == '÷' || lastChar == '^')) {
                                    currentStr += command;
                                } else if (Character.isDigit(command.charAt(0))) {
                                    currentStr += command;
                                } else if (isOperator(command.charAt(0))) {
                                    currentStr = currentStr.substring(0, currentStr.length() - 1) + command;
                                }
                            } else {
                                if (command.equals(".")) {
                                    if (!currentStr.contains(".") && Character.isDigit(lastChar)) {
                                        currentStr += command;
                                    } else if (currentStr.isEmpty() || currentStr.equals("-")) {
                                        currentStr += "0.";
                                    }
                                } else {
                                    currentStr += command;
                                }
                            }
                        }

                        tf.setText(currentStr);
                    }
                }
            });
        }
    }

    public static String getAnswer(String expression) {
        Stack<Double> numbers = new Stack<>();
        Stack<Character> operations = new Stack<>();
        
        for (int i = 0; i < expression.length(); i++) {
            char c = expression.charAt(i);

            // 숫자 및 음수 숫자 처리
            if (Character.isDigit(c) || c == '.' || (c == '-' && (i == 0 || isOperator(expression.charAt(i - 1)) || expression.charAt(i - 1) == '('))) {
                StringBuilder sb = new StringBuilder();
                if (c == '-') {
                    sb.append(c);
                    i++;
                }
                while (i < expression.length() && (Character.isDigit(expression.charAt(i)) || expression.charAt(i) == '.')) {
                    sb.append(expression.charAt(i));
                    i++;
                }
                i--;
                numbers.push(Double.parseDouble(sb.toString()));
            } else if (c == '(') {
                operations.push(c);
            } else if (c == ')') {
                while (operations.peek() != '(') {
                    double value = applyOp(operations.pop(), numbers.pop(), numbers.pop());
                    numbers.push(value);
                }
                operations.pop();
            } else if (isOperator(c)) {
                while (!operations.isEmpty() && precedence(c) <= precedence(operations.peek())) {
                    numbers.push(applyOp(operations.pop(), numbers.pop(), numbers.pop()));
                }
                operations.push(c);
            } else if (c == '%') {
                double num = numbers.pop();
                numbers.push(getPercentage(num));
            } else if (c == '!') {
                double num = numbers.pop();
                numbers.push(getFactorial((int) num));
            } else if (c == 'l' && i + 2 < expression.length() && expression.substring(i, i + 3).equals("log")) {
                i += 2;
                double num = numbers.pop();
                if (num <= 0) return "Error"; // 음수 로그 처리
                numbers.push(getLog(num));
            } else if (c == 'l' && i + 1 < expression.length() && expression.substring(i, i + 2).equals("ln")) {
                i += 1;
                double num = numbers.pop();
                if (num <= 0) return "Error"; // 음수 자연 로그 처리
                numbers.push(getNatureLog(num));
            } else if (c == 'r' && i + 3 < expression.length() && expression.substring(i, i + 4).equals("root")) {
                i += 3;
                double num = numbers.pop();
                if (num < 0) return "Error"; // 음수 제곱근 처리
                numbers.push(getRootValue(num));
            } else if (c == 's' && i + 2 < expression.length() && expression.substring(i, i + 3).equals("sin")) {
                i += 2;
                double num = numbers.pop();
                numbers.push(Math.sin(Math.toRadians(num)));
            } else if (c == 'c' && i + 2 < expression.length() && expression.substring(i, i + 3).equals("cos")) {
                i += 2;
                double num = numbers.pop();
                numbers.push(Math.cos(Math.toRadians(num)));
            } else if (c == 't' && i + 2 < expression.length() && expression.substring(i, i + 3).equals("tan")) {
                i += 2;
                double num = numbers.pop();
                numbers.push(Math.tan(Math.toRadians(num)));
            }
        }

        while (!operations.isEmpty() && numbers.size() >= 2) {
            double secondOperand = numbers.pop();
            double firstOperand = numbers.pop();
            numbers.push(applyOp(operations.pop(), secondOperand, firstOperand));
        }

        double result = numbers.pop();
        String formattedResult;
        
        if (result % 1 == 0) {
            if (result > 1e11 || result < -1e11) {
                formattedResult = String.format("%e", result);
            } else {
                formattedResult = String.format("%.0f", result);
            }
        } else {
            int integerPartLength = String.valueOf((long) result).length();
            int decimalPrecision = 12 - integerPartLength;
            if (decimalPrecision < 0) decimalPrecision = 0;
            formattedResult = String.format("%." + decimalPrecision + "f", result);
            formattedResult = formattedResult.replaceAll("0*$", "").replaceAll("\\.$", "");
        }
        
        return formattedResult;
    }


    public static boolean isOperator(char op) {
        return op == '+' || op == '-' || op == '×' || op == '÷' || op == '^';
    }

    public static int precedence(char op) {
        switch (op) {
            case '+':
            case '-':
                return 1;
            case '×':
            case '÷':
                return 2;
            case '^':
            	return 3;
        }
        return -1;
    }

    public static double applyOp(char op, double b, double a) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '×': return a * b;
            case '÷': return a / b;
            case '^': return Math.pow(a, b);
        }
        return 0;
    }

    public static double getPercentage(double num) {
        return num / 100;
    }
    
    public static double getFactorial(int num) {
        int i;
        double temp = 1;
        
        for (i = num; i >= 1; i--) {
            temp *= i;
        }
        
        return temp;
    }

    public static double getLog(double num) {
        return Math.log10(num);
    }

    public static double getNatureLog(double num) {
        return Math.log(num);
    }

    public static double getRootValue(double num) {
        return Math.sqrt(num);
    }
}
