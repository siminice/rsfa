import com.ibm.icu.text.Transliterator;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class BlrPlayer {
  static String mnem = "";
  static String index = "-1";
  static Map<String, String> codes = new HashMap();
  static SimpleDateFormat hudf = new SimpleDateFormat("dd.MM.yyyy");
  static SimpleDateFormat isof = new SimpleDateFormat("yyyy-MM-dd");
  static Transliterator xlit = Transliterator.getInstance("Cyrillic-Latin");

  public static void main(String args[]) {
    File input = new File(args[0]);
    mnem = args[1];
    Document doc = null;
    try {
      codes = loadCountryCodes("/var/www/html/country-ru.dat");
    } catch (Exception e) {
      System.err.println("Error: country codes not loaded." + e.getMessage());
    }
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://footballfacts.ru");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element head = doc.selectFirst(".elgg-heading-main");
    String cname = "";
    if (head!=null && head.childNodeSize()>0) cname = head.childNode(0).toString();
    Element content = doc.selectFirst(".elgg-main");
    extractBio(content, cname);
  }

  private static void extractBio(Element content, String cname) {

    Elements enames = content.select(".personname");
    String lastrn = "";
    String firsrn = "";
    String lastln = lastrn;
    String firsln = firsrn;
    if (enames.size() == 0) {
      String rparts[] = cname.split(" ");
      lastrn = rparts[0];
      StringBuilder sbf = new StringBuilder();
      if (rparts.length>1) sbf.append(rparts[1]);
      for (int k=2; k<rparts.length; k++) {
        sbf.append(" "); sbf.append(rparts[k]);
      }
      firsrn = sbf.toString();
    } else {
      Element latname = enames.get(0);
      if (enames.size() > 1) {
        latname = enames.get(1);
        if (enames.size() > 2) {
          for (Element ename : enames) {
            Element pname = ename.parent().selectFirst(":contains(Латиница)");
            if (pname == null) continue;
            else latname = pname;
          }
        }
        Element elastl = latname.selectFirst(".sname");
        Element efirsl = latname.selectFirst(".fname");
        lastln = elastl != null ? elastl.childNode(0).toString().trim() : "";
        firsln = efirsl != null ? efirsl.childNode(0).toString().trim() : "";
      }
    }

    Element eb = content.selectFirst("b:contains(Родился)");
    String dob = "0000-00-00";
    String isoDob = "0000-00-00";
    String pob = " ";
    String lcob = "BLR";
    if (eb!=null) {
      eb = eb.parent();
      Element edob = eb.selectFirst("nobr");
      dob = edob.childNode(0).toString().replaceAll("\n", "");
      isoDob = toIso(dob);

      Elements places = eb.select("a[href]");
      if (places.size() > 1) {
        Element ecity = places.get(0);
        pob = ecity != null && ecity.childNodeSize() > 0 ?
            ecity.childNode(0).toString().trim().replaceAll("г. ", "") : " ";
        Element el = places.get(places.size()-1);
        Element elcob = el.selectFirst(".contryname");
        if (elcob!=null) lcob = elcob.childNode(0).toString().trim();
      }
    }

    Element enat = content.selectFirst("b:contains(Гражданство)");
    String lnat = "BLR";
    if (enat != null) {
      enat = enat.parent();
      Element ectty = enat.selectFirst(".contryname");
      lnat = ectty!=null && ectty.childNodeSize() > 0 ? ectty.childNode(0).toString().trim() : "BLR";
    }

    String nat = codes.values().contains(lnat) ? lnat : codes.getOrDefault(lnat, " ");
    String cob = codes.values().contains(lcob) ? lcob : codes.getOrDefault(lcob, " ");

    String rname = firsrn + " " + lastrn;
    String lname = firsln + " " + lastln;
    String fname = lname;
    if (fname.length() <= 1) {
      String xname = xlit.transliterate(rname);
      String fxname = xform(xname);
      String xparts[] = fxname.split(" ");
      int nx = xparts.length;
      if (nx > 0) fname = xparts[0] + " " + xparts[nx-1];
    }
    System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s,%s",
        mnem, fname, rname, isoDob, nat, pob, cob, index));
  }


  private static String toIso(String dob) {
    if (dob==null) return "0000-00-00";

    try {
      Date d = hudf.parse(dob);
      return isof.format(d);

    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

  private static boolean isDate(String s) {
    SimpleDateFormat hudf = new SimpleDateFormat("yyyy.MM.dd");
    try {
      hudf.parse(s);
    } catch (ParseException e) {
      return false;
    }
     return true;
  }

  private static Map<String, String> loadCountryCodes(String filename) throws IOException {
    Charset charset = Charset.forName("UTF-8");
    List<String> lines = Files.readAllLines(Paths.get(filename), charset);
    Map<String, String> codes = new HashMap();
    for (String s : lines) {
      String[] parts = s.split(",");
      if (parts.length > 1) {
        codes.put(parts[1], parts[0]);
      }
    }
    return codes;
  }

  private static String xform(String s) {
    s.replaceAll("C", "Ts");
    s.replaceAll("c", "ts");
    s.replaceAll("H", "Kh");
    s.replaceAll("í", "i");
    s.replaceAll("j", "y");
    s.replaceAll("Č", "Ch");
    s.replaceAll("č", "ch");
    s.replaceAll("š", "sh");
    s.replaceAll("Š", "Sh");
    s.replaceAll("ŝ", "shch");
    s.replaceAll("Ŝ", "Shch");
    s.replaceAll("Š", "Sh");
    s.replaceAll("Û", "Yu");
    s.replaceAll("û", "yu");
    s.replaceAll("í", "i");
    s.replaceAll("â", "ya");
    s.replaceAll("Ž", "Zh");
    return s;
  }
}

